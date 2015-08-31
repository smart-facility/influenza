/****************************************************************
 * MODEL.CPP
 *
 * This file contains all the definitions of the methods of
 * Model.hpp (see this file for methods' documentation)
 *
 * Authors: J. Barthelemy
 * Date   : 11 june 2014
 ****************************************************************/

#include "../include/Model.hpp"

using namespace repast;
using namespace std;
using namespace tinyxml2;

Model::Model( boost::mpi::communicator* world, Properties & props ) : _props(props) {

	// Reading properties, rank of the process and input filenames ----

	_proc  = RepastProcess::instance()->rank();
	agents = new SharedContext<Individual>(world);

	// Model space initialization -------------------------------------

	_network = Data::getInstance()->getNetwork();

	// Process nodes recording ----------------------------------------

	for( auto n : _network.getNodes() ) {
		_map_node_process[n.first] = _proc;
	}
	constructMapNodeProcess();

	// Initialization of the agents ----------------------------------

	init_agents_sax();
	cout << "INFO: Proc " << _proc << ": Number of agents: " << agents->size() << endl;

	// Init agents sick
	int n_infected = boost::lexical_cast<int>(_props.getProperty("n.infected"));
	initInfectAgents(n_infected);

	// Aggregate data output ------------------------------------------

	string fileOutputName("../output/sim_out.csv");
	SVDataSetBuilder builder( fileOutputName.c_str(), ";", RepastProcess::instance()->getScheduleRunner().schedule() );
	builder.addDataSource(repast::createSVDataSource("total_agents", &this->_total_agents, std::plus<int>()));
	builder.addDataSource(repast::createSVDataSource("total_agents_infected", &this->_total_infected, std::plus<int>()));
	builder.addDataSource(repast::createSVDataSource("total_nodes_infected", &this->_total_nodes_infected, std::plus<int>()));
	this->_data_collection = builder.createDataSet();

	if ( _proc == 0 ) cout << "... end of model initialization!" << endl;

}


Model::~Model() {
	delete agents;
}

void Model::init_agents_sax() {

	VBSaxParser parser(_proc, *this);
	string input_xml_file = this->_props.getProperty("file.agenda");
	try
	{
		parser.set_substitute_entities(true);
		parser.parse_file(input_xml_file);
	}
	catch(const xmlpp::exception& ex)
	{
		std::cout << "libxml++ exception: " << ex.what() << std::endl;
	}

}

void Model::synch_agents() {

	repast::RepastProcess::instance()->synchronizeAgentStatus<Individual,IndividualPackage,Model,Model,Model>(*this->agents, *this, *this, *this);
}


void Model::initSchedule() {

	// Initialize the scheduler
	ScheduleRunner & runner = RepastProcess::instance()->getScheduleRunner();

	// Call the step method on the Model every tick
	runner.scheduleEvent(1, 1, Schedule::FunctorPtr(new MethodFunctor<Model>(this, &Model::step)));

	// Stopping the model when reaching the desired number of iteration
	int stop_at = repast::strToInt(_props.getProperty("stop"));
	runner.scheduleStop(stop_at);

	// Schedule the aggregate data recording and writing
	runner.scheduleEndEvent(Schedule::FunctorPtr(new MethodFunctor<DataSet>(_data_collection, &DataSet::write)));

}

void Model::providePackage(Individual * agent, std::vector<IndividualPackage>& out) {

	AgentId id = agent->getId();
	IndividualPackage package = { id.id(), id.startingRank(), id.agentType(), id.currentRank(),
			agent->getAgenda(), agent->getCurAct(), agent->getAgeCl(), agent->getGender(), agent->getSocioProStatus(),
			agent->getEduLevel(), agent->isSick() };
	out.push_back(package);

}


Individual * Model::createAgent(IndividualPackage package) {

	repast::AgentId id(package.id, package.init_proc, MODEL_AGENT_IND_TYPE, package.cur_proc);
	return new Individual(id, package.agenda, package.cur_act, package.age_cl, package.gender,
			package.socio_pro_status, package.edu_level, package.sick);

}


void Model::provideContent(repast::AgentRequest req, std::vector<IndividualPackage>& out) {

	std::vector<AgentId> ids = req.requestedAgents();
	for( unsigned int i = 0; i < ids.size(); i++ ) {
		providePackage(this->agents->getAgent(ids[i]),out);
	}

}


void Model::updateAgent(IndividualPackage package) {

	repast::AgentId id(package.id, package.init_proc, MODEL_AGENT_IND_TYPE);
	Individual * agent = this->agents->getAgent(id);
	agent->getId().currentRank(package.cur_proc);
	agent->setAgenda(package.agenda);
	agent->setAgeCl(package.age_cl);
	agent->setGender(package.gender);
	agent->setSocioProStatus(package.socio_pro_status);
	agent->setEduLevel(package.edu_level);
	agent->setSick(package.sick);

}



void Model::step() {

	// convert current tick to int!
	int tick = boost::lexical_cast<int>(repast::RepastProcess::instance()->getScheduleRunner().currentTick());

	// reset counters
	_total_infected.setData(0);

	_map_agents_to_move_process.clear();

	// Loop over every agents

	repast::SharedContext<Individual>::const_local_iterator it_agent = (*agents).localBegin();
	while( it_agent != (*agents).localEnd()) {

		int start_time_cur_act = (*it_agent)->getCurActStartingTime();
		int end_time_cur_act   = (*it_agent)->getCurActEndTime();
		long cur_node = (*it_agent)->getNodeId();

		// if tick = start_time                            : if node not infected and agent already infected -> infect the node the node; if node and agent not infected: do nothing
		if( tick == start_time_cur_act ) {
			if( (*it_agent)->isSick() == true &&  _network.isNodeInfected(cur_node) == false ) {
				_network.addInfectedNode(cur_node);
			}
			// updating the contact of the other ind in the node
			// updating the list of individual on the node (this method will also update the infection status of the node)
		}

		// if start_time_cur_act <= tick < end_time_cur_act : check if node is currently infected; if so get infected
		if( start_time_cur_act <= tick && tick <= end_time_cur_act ) {
			if( (*it_agent)->isSick() == false && _network.isNodeInfected(cur_node) == true ) {
				(*it_agent)->setSick(true);
				_network.addInfectedNode(cur_node);
			}
		}


		if( (*it_agent)->isSick() == true ) {
			_total_infected.incrementData();
		}

		// if tick > end_time_cur_act : remove agent from the node and cur_act++, prepare agent next activity and move it to the good process

		if( tick > end_time_cur_act ) {

			// remove agent from current node

			// if no more activities, remove the agent from the context
			bool set_next_act = (*it_agent)->setNextAct();
			if( set_next_act == true ) {
				if( (*it_agent)->isSick() == true ) {
					_network.removeInfectedNode(cur_node);
				}
				// check if the node of the next activity belongs to the
				int dest_proc = _map_node_process[(*it_agent)->getNodeId()];
				if( dest_proc != _proc ) {
					repast::RepastProcess::instance()->moveAgent( (*it_agent)->getId(), dest_proc);
				}
				//agents->removeAgent((*it_agent)->getId()); // it would be better not to remove the agents
			}

		}

		// next agent
		it_agent++;

	}

	// Recording aggregate data
	_total_nodes_infected.setData(_network.getNInfectedNodes());
	_total_agents.setData(this->agents->size());
	_data_collection->record();

	synch_agents();

	if( _proc == 0 && tick % 3600 == 0) {
		cout << "INFO: TICK " << tick / 3600 <<  " done" << endl;
	}

}


void Model::constructMapNodeProcess() {

	boost::mpi::communicator* comm = RepastProcess::instance()->getCommunicator();

	// gathering data from every process
	vector<map<long, int>> result_gather;
	boost::mpi::all_gather(*comm, _map_node_process, result_gather);

	// updating map with gathered data
	for( unsigned int i = 0; i < result_gather.size(); i++ ) {
		_map_node_process.insert( result_gather[i].begin(), result_gather[i].end() );
	}

}


bool Model::isInLocalBounds(long nodeId) {

	if( this->_network.getNodes().count(nodeId) > 0 ) {
		return true;
	}
	return false;

}
const std::map<long, int>& Model::getMapNodeProcess() const {
	return _map_node_process;
}


void Model::initInfectAgents(int n) {

	// check if there are enough agents in the SharedContext
	if( agents->size() > 0 ) {
		if( n > agents->size() ) n = agents->size();
		std::vector<Individual*> agents_to_infect;
		agents->getRandomAgents(n, agents_to_infect);
		// infect random agents and update their node status
		for( auto agt : agents_to_infect ) {
			agt->setSick(true);
			_network.addInfectedNode(agt->getNodeId());
		}
	} else {
		n = 0;
	}

	cout << "INFO: Proc " << _proc << ": Number of agents infected: " << n << endl;

}

void Model::infectAgent(Individual *ind) {

	// set individual status to sick
	ind->setSick(true);

	// update the number of infected people on the node where the individual is
	_network.addInfectedNode(ind->getNodeId());

	// update the number of sick people
	_total_infected.incrementData();

}
