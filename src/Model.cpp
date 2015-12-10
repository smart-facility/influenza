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

Model::Model( boost::mpi::communicator* world, Properties & props ) : _props(props) {

	// Reading properties, rank of the process and input filenames ----

	_proc  = RepastProcess::instance()->rank();
	_agents = new SharedContext<Individual>(world);

	// Model space initialization -------------------------------------

	// getting the network
	_network = Data::getInstance()->getNetwork();

	// dumping nodes in a file
	//_network.dumpNodes();

	// process nodes recording
	for( auto n : _network.getNodes() ) {
		_map_node_process[n.first] = _proc;
	}
	constructMapNodeProcess();

	// Spatial projection construction --------------------------------

	int n_nodes = _map_node_process.size();
	if (_proc == 0 ) {
		cout << "INFO: MODEL CONSTRUCTOR: total number of nodes: " << n_nodes << endl;
	}

	Point<double> origin(0,0);
	Point<double> extent(n_nodes,1);
	GridDimensions grid_dim(origin, extent);
	vector<int> process_dims;
	process_dims.push_back(world->size());
	process_dims.push_back(1);

	_discrete_space = new SharedDiscreteSpace<Individual, WrapAroundBorders,SimpleAdder<Individual>>("AgentDiscreteSpace", grid_dim, process_dims, 0, world);
	_agents->addProjection(_discrete_space);

	cout << "INFO: Proc " << _proc << ": Dimensions: " << _discrete_space->dimensions() << endl;

	// Model parameters -----------------------------------------------

	_r_beta      = boost::lexical_cast<float>(_props.getProperty("r.beta"));
	_beta        = boost::lexical_cast<float>(_props.getProperty("beta"));
	_epsilon = 1 / boost::lexical_cast<float>(_props.getProperty("epsilon.inv"));
	_p_a         = boost::lexical_cast<float>(_props.getProperty("p.a"));
	_mu      = 1/  boost::lexical_cast<float>(_props.getProperty("mu.inv"));
	_max_inf     = boost::lexical_cast<float>(_props.getProperty("max.inf"));

	_r_beta_x_beta = _r_beta * _beta;

	// Random generators --------------------------------------------

	initializeRandom(props, world);
	ExponentialGenerator* rnd_epsilon_inv = new ExponentialGenerator(Random::instance()->createExponentialGenerator(_epsilon));
	Random::instance()->putGenerator("epsilon",rnd_epsilon_inv);
	ExponentialGenerator* rnd_mu_inv = new ExponentialGenerator(Random::instance()->createExponentialGenerator(_mu));
	Random::instance()->putGenerator("mu",rnd_mu_inv);

	// Initialization of the agents -----------------------------------

	init_agents_sax();
	cout << "INFO: Proc " << _proc << ": Number of agents: " << _agents->size() << endl;

	// Init agents sick
	initInfectAgents();

	// Aggregate data output ------------------------------------------

	string fileOutputName("../output/sim_out.csv");
	SVDataSetBuilder builder( fileOutputName.c_str(), ";", RepastProcess::instance()->getScheduleRunner().schedule() );
	builder.addDataSource(repast::createSVDataSource("total_susceptible", &this->_total_susceptible, std::plus<int>()));
	builder.addDataSource(repast::createSVDataSource("total_latent", &this->_total_latent, std::plus<int>()));
	builder.addDataSource(repast::createSVDataSource("total_asymptomatic", &this->_total_infectious_asympt, std::plus<int>()));
	builder.addDataSource(repast::createSVDataSource("total_symptomatic", &this->_total_infectious_sympt, std::plus<int>()));
	builder.addDataSource(repast::createSVDataSource("total_recovered", &this->_total_recovered, std::plus<int>()));
	builder.addDataSource(repast::createSVDataSource("total_nodes_infected", &this->_total_nodes_infected, std::plus<int>()));
	this->_data_collection = builder.createDataSet();

	if ( _proc == 0 ) cout << "... end of model initialization!" << endl;

}


Model::~Model() {
	delete _agents;
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
		cerr << "libxml++ exception: " << ex.what() << endl;
	}

}


void Model::synch_agents() {

	/*
	for(auto a : _map_agents_to_move_process) {
		cout << "INFO: SYNC - Proc " << _proc << " sending agent " << a.first.id() << " to proc " << a.second << endl;
	}
	 */


	_discrete_space->balance(_map_agents_to_move_process);
	repast::RepastProcess::instance()->synchronizeAgentStatus<Individual,IndividualPackage,Model,Model,Model>(*this->_agents, *this, *this, *this);

}


void Model::initSchedule() {

	// Initialize the scheduler
	ScheduleRunner & runner = RepastProcess::instance()->getScheduleRunner();

	// Call the step method on the Model every tick
	runner.scheduleEvent(1, 1, Schedule::FunctorPtr(new MethodFunctor<Model>(this, &Model::resetDataInd)));
	runner.scheduleEvent(1, 1, Schedule::FunctorPtr(new MethodFunctor<Model>(this, &Model::step)));

	// Stopping the model when reaching the desired number of iteration
	int stop_at = repast::strToInt(_props.getProperty("stop"));
	runner.scheduleStop(stop_at);

	// Schedule the aggregate data recording and writing
	//runner.scheduleEvent(1, 1, Schedule::FunctorPtr(new MethodFunctor<DataSet>(_data_collection, &DataSet::record)));
	//runner.scheduleEndEvent(Schedule::FunctorPtr(new MethodFunctor<DataSet>(_data_collection, &DataSet::record)));

	runner.scheduleEndEvent(Schedule::FunctorPtr(new MethodFunctor<DataSet>(_data_collection, &DataSet::write)));

}


void Model::providePackage(Individual * agent, std::vector<IndividualPackage>& out) {

	AgentId id = agent->getId();
	IndividualPackage package = { id.id(), id.startingRank(), id.agentType(), id.currentRank(),
			agent->getAgenda(), agent->getCurAct(), agent->getAgeCl(), agent->getGender(), agent->getSocioProStatus(),
			agent->getEduLevel(), agent->getState(), agent->getTimeTransition() };
	out.push_back(package);

}


Individual * Model::createAgent(IndividualPackage package) {

	repast::AgentId id(package.id, package.init_proc, MODEL_AGENT_IND_TYPE, package.cur_proc);
	return new Individual(id, package.agenda, package.cur_act, package.age_cl, package.gender,
			package.socio_pro_status, package.edu_level, package.state, package.time_next_state);

}


void Model::provideContent(repast::AgentRequest req, std::vector<IndividualPackage>& out) {

	std::vector<AgentId> ids = req.requestedAgents();
	for( unsigned int i = 0; i < ids.size(); i++ ) {
		providePackage(this->_agents->getAgent(ids[i]),out);
	}

}


void Model::updateAgent(IndividualPackage package) {

	repast::AgentId id(package.id, package.init_proc, MODEL_AGENT_IND_TYPE);
	Individual * agent = this->_agents->getAgent(id);
	agent->getId().currentRank(package.cur_proc);
	agent->setAgenda(package.agenda);
	agent->setAgeCl(package.age_cl);
	agent->setGender(package.gender);
	agent->setSocioProStatus(package.socio_pro_status);
	agent->setEduLevel(package.edu_level);
	agent->setState(package.state);

}


void Model::step() {

	// convert current tick to current time of day (in seconds from midnight)
	// int tick = boost::lexical_cast<int>(repast::RepastProcess::instance()->getScheduleRunner().currentTick());
	static int time_of_day    = 0;
	static int days_simulated = 0;
	if( time_of_day > 86400 ) {
		time_of_day = time_of_day - 86400;
		days_simulated++;
		cout << "INFO: DAY " << days_simulated << " IS DONE on Proc " << repast::RepastProcess::instance()->rank() << endl;
	}

	time_of_day++;

	// clearing the map containing the agents to be moved between processes
	_map_agents_to_move_process.clear();

	// querry method applied on the discrete space
	Moore2DGridQuery<Individual> moore2DQuery(_discrete_space);

	// Loop over every agents

	auto it_agent = (*_agents).localBegin();
	while( it_agent != (*_agents).localEnd()) {

		// check if agent is latent and should become (asymptomic) infectious
		if( (*it_agent)->getState() == state_inf::LATENT ) {
			(*it_agent)->decreaseTimeTransition();
			if( (*it_agent)->getTimeTransition() == 0 ) {
				(*it_agent)->determineInfectiousType(_p_a);
				//cout << "INFO: TICK " << time_of_day << ", Proc " << _proc << ": Agent " << (*it_agent)->getId().id() << " moves from LATENT to " << (*it_agent)->getState() << endl;
			}
		}

		// check if agent is infectious and should recover
		if( (*it_agent)->getState() == state_inf::INFECTIOUS_ASYMPT
				|| (*it_agent)->getState() == state_inf::INFECTIOUS_SYMPT ) {
			(*it_agent)->decreaseTimeTransition();
			if( (*it_agent)->getTimeTransition() == 0 ) {
				(*it_agent)->setState(state_inf::RECOVERED);
				//cout << "INFO: TICK " << time_of_day << ", Proc " << _proc << ": Agent " << (*it_agent)->getId().id() << " moves from INFECTED to " << (*it_agent)->getState() << endl;
			}
		}

		//  check current activity times
		int start_time_act = (*it_agent)->getCurActStartingTime();
		int end_time_act   = (*it_agent)->getCurActEndTime();
		vector<int> agt_location;
		_discrete_space->getLocation((*it_agent)->getId(),agt_location);

		// if agent is infected, queries the agents on the same spot to tries to infect them
		if( (*it_agent)->getState() == state_inf::INFECTIOUS_ASYMPT
				|| (*it_agent)->getState() == state_inf::INFECTIOUS_SYMPT ) {

			// ... recording infected node
			_network.addInfectedNode(agt_location[0]);

			// ... agents are performing an activity somewhere
			if (time_of_day <= end_time_act && start_time_act <= time_of_day && agt_location[1] == 0) {

				// queries agent on a node
				vector<Individual*> agents_on_node;
				Point<int> node_location(agt_location[0], 0);
				moore2DQuery.query(node_location, 0, true, agents_on_node);

				// loop on the agents
				int n_interactions = 0;
				auto agt = agents_on_node.begin();
				while( n_interactions < _max_inf && agt != agents_on_node.end() ) {

					// only infect the susceptible agents
					if( (*agt)->getState() == state_inf::SUSCEPTIBLE ) {

						//bool latent = false;
						if( (*it_agent)->getState() == state_inf::INFECTIOUS_ASYMPT ) {
							//latent =
							(*agt)->isLatent( _r_beta_x_beta );

						} else {
							//latent =
							(*agt)->isLatent( _beta );
						}

						/*
						if( latent == true ) {
							cout << "INFO: TICK " << time_of_day << ", Proc " << _proc << ": Agent " << (*it_agent)->getId().id() << " moves from SUSCEPTIBLE to " << (*agt)->getState() << endl;
						}
						*/

					}

					agt++;
					n_interactions++;

				}

			}

		}

		// the agent should be paused until next activity
		if( end_time_act == time_of_day ) {
			// ... removing it from its current location
			//cout << "INFO: TICK " << time_of_day << ", Proc " << _proc << ": Agent " << (*it_agent)->getId().id() << " END ACT, SCHEDULE NEXT ONE!" << endl;
			agt_location[1] = 1;
			_discrete_space->moveTo((*it_agent)->getId(),agt_location);
			// ... setting next activity
			bool has_next_activity = (*it_agent)->setNextAct();

			// ... if no more activity then reset the schedule
			if ( has_next_activity == false ) {
				//cout << "INFO: TICK " << time_of_day << ", Proc " << _proc << ": Agent " << (*it_agent)->getId().id() << " RESTART SCHEDULE!" << endl;
				bool is_active = (*it_agent)->resetSchedule(time_of_day);
				if( is_active ) {
					agt_location[0] = (*it_agent)->getCurActNodeId();
					agt_location[1] = 0;
					_discrete_space->moveTo( (*it_agent)->getId(), agt_location);

					// ... checking if the agent needs to be moved to another process
					if( isInLocalBounds(agt_location[0]) == false ) {
						_map_agents_to_move_process[(*it_agent)->getId()] = _map_node_process[agt_location[0]];
					}
				}
			}

		}

		// the agent will resume at the next tick and be moved to next activity
		if( start_time_act == time_of_day - 1 ) {
			 // ... moving it to the right location
			 agt_location[0] = (*it_agent)->getCurActNodeId();
			 agt_location[1] = 0;
		     _discrete_space->moveTo( (*it_agent)->getId(), agt_location);

		     // ... checking if the agent needs to be moved to another process
		     if( isInLocalBounds(agt_location[0]) == false ) {
		    	 _map_agents_to_move_process[(*it_agent)->getId()] = _map_node_process[agt_location[0]];
		     }

		}

		// aggregate data
		Individual& ind = *(*it_agent);
		gatherDataInd( ind );

		// next agent
		it_agent++;

	}

	// Recording aggregate data
	_total_nodes_infected.setData(_network.getNInfectedNodes());
	_data_collection->record();

	synch_agents();

	if( time_of_day % 3600 == 0) {
		std::ostringstream screen_output;
		screen_output << "INFO: HOUR " << time_of_day / 3600 << " done on Proc " << repast::RepastProcess::instance()->rank() << " (" << _agents->size() << " agents)" << endl;
		std::cout << screen_output.str();
	}

}


void Model::constructMapNodeProcess() {

	boost::mpi::communicator* comm = RepastProcess::instance()->getCommunicator();

	// gathering data from every process
	vector<map<int, int>> result_gather;
	boost::mpi::all_gather(*comm, _map_node_process, result_gather);

	// updating map with gathered data
	for( unsigned int i = 0; i < result_gather.size(); i++ ) {
		_map_node_process.insert( result_gather[i].begin(), result_gather[i].end() );
	}

}


bool Model::isInLocalBounds(int nodeId) {

	if( this->_network.getNodes().count(nodeId) > 0 ) {
		return true;
	}
	return false;

}


const std::map<int, int>& Model::getMapNodeProcess() const {
	return _map_node_process;
}


void Model::initInfectAgents() {

	vector<int> node_infect_sympt = split<int>(_props.getProperty("infection.sympt.starting.node"),",");
	vector<int> n_infect_sympt_th = split<int>(_props.getProperty("n.infected.sympt"),",");
	initInfectAgents(node_infect_sympt, n_infect_sympt_th, state_inf::INFECTIOUS_SYMPT);

	vector<int> node_infect_asympt = split<int>(_props.getProperty("infection.asympt.starting.node"),",");
	vector<int> n_infect_asympt_th = split<int>(_props.getProperty("n.infected.asympt"),",");
	initInfectAgents(node_infect_asympt,n_infect_asympt_th,state_inf::INFECTIOUS_ASYMPT);

}


void Model::initInfectAgents(std::vector<int> nodeIds, std::vector<int> nAgents, state_inf state) {

	for(unsigned int i = 0; i < nodeIds.size(); i++) {

		// how many agents and where to get them
		int n_infect_th = nAgents[i];
		int node_orig_id = nodeIds[i];
		int node_id = Data::getInstance()->getMapNodesOrigIdNewId().at(node_orig_id);

		// queries agent on a node
		vector<Individual*> agents_on_node;
		Point<int> node_location(node_id, 0);
		Moore2DGridQuery<Individual> moore2DQuery(_discrete_space);
		moore2DQuery.query(node_location, 0, true, agents_on_node);

		if( _map_node_process.at(node_id) == _proc ) {

			cout << "INFO: INFECT AGENTS - " << state << " - node " << node_orig_id << " (" << node_id << ")" << endl;

			// infection
			int n_infect = 0;
			auto agt = agents_on_node.begin();
			while( n_infect < n_infect_th && agt != agents_on_node.end() ) {

				// only infect the suscpetible agents
				if( (*agt)->getState() == state_inf::SUSCEPTIBLE ) {
					n_infect++;
					(*agt)->setState(state);
					(*agt)->setTimeTransition((int)(Random::instance()->getGenerator("mu")->next() * 86400));
					(*agt)->print();
				}
				agt++;

			}

			cout << "INFO: Proc " << _proc << ": Number of agents infected (" << state <<  "): " << n_infect << " (desired: " << n_infect_th << ")" << endl;

		}

	}

}


void Model::gatherDataInd(const Individual& aInd) {

	switch(aInd.getState()) {
	case state_inf::SUSCEPTIBLE:
		_total_susceptible.incrementData();
		break;
	case state_inf::LATENT:
		_total_latent.incrementData();
		break;
	case state_inf::INFECTIOUS_SYMPT:
		_total_infectious_sympt.incrementData();
		break;
	case state_inf::INFECTIOUS_ASYMPT:
		_total_infectious_asympt.incrementData();
		break;
	case state_inf::RECOVERED:
		_total_recovered.incrementData();
		break;
	default:
		cerr << "ERROR: Proc " << _proc << ": Not a valid state for individual " << aInd.getId().id() << endl;
	}

}


void Model::resetDataInd() {
	_total_susceptible.resetData();
	_total_latent.resetData();
	_total_infectious_asympt.resetData();
	_total_infectious_sympt.resetData();
	_total_recovered.resetData();
}
