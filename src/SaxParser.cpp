#include "../include/SaxParser.hpp"
#include "../include/Model.hpp"


VBSaxParser::VBSaxParser(int aProc, Model& aModel)
: xmlpp::SaxParser(), _proc(aProc), _model(aModel) {
}

VBSaxParser::~VBSaxParser() {
}

void VBSaxParser::on_start_document() {
	std::cout << "Starting xml parsing!" << std::endl;
}

void VBSaxParser::on_end_document() {
	std::cout << "Parsing xml done" << std::endl;
}

//Error while parsing the xml file: rewrite this function, otherwise we miss the last individual
//maybe playing in the part with activity: when generating an activity:
//     -> look if first
//     -> adding the ind to the context
void VBSaxParser::on_start_element(const Glib::ustring& name, const AttributeList& attributes) {

	static Individual* cur_ind = NULL;

	if( name.compare("person") == 0 ) {
		cur_ind = on_individual(attributes);
	}

	// generating the agenda of the current individual
	if( name.compare("act") == 0 ) {
		on_activity(attributes, cur_ind);

		// if activity = m and associated node belongs to the current proc, add the agent to the context
		if( _model.getMapNodeProcess().at(cur_ind->getHouseNodeId()) == _proc
				&& cur_ind->getAgenda().size() == 1) {
			_model.addAgent(cur_ind);
			_model.moveAgentToNode(cur_ind->getId(),cur_ind->getHouseNodeId());
		}

	}

}

Individual* VBSaxParser::on_individual(const AttributeList &attributes) {

	// reading the agents attributes

	int id;
	int age_cl;
	char gender;
	char socio_pro_status;
	char edu_level;

	for(xmlpp::SaxParser::AttributeList::const_iterator iter = attributes.begin(); iter != attributes.end(); ++iter) {
		if(iter->name.raw().compare("id")         == 0) id               = boost::lexical_cast<int>(iter->value.raw());
		if(iter->name.raw().compare("gender")     == 0) gender           = boost::lexical_cast<char>(iter->value.raw());
		if(iter->name.raw().compare("age_cl")     == 0) age_cl           = boost::lexical_cast<unsigned int>(iter->value.raw());
		if(iter->name.raw().compare("education")  == 0) edu_level        = boost::lexical_cast<char>(iter->value.raw());
		if(iter->name.raw().compare("sps_status") == 0) socio_pro_status = boost::lexical_cast<char>(iter->value.raw());
	}

	// creating agent_id
	repast::AgentId repast_id(id, _proc, MODEL_AGENT_IND_TYPE, _proc);

	// creating the agent
	Individual *cur_ind = new Individual(repast_id,age_cl,gender,socio_pro_status,edu_level);

	return cur_ind;

}

void VBSaxParser::on_activity(const AttributeList &attributes, Individual *ind ) {

	char type;
	int node_id = -1; // -1 indicates that it is the last activity of the day, ie return to home
	int duration = -1;
	int end_time = -1;

	static int house_id = -1;

	for(xmlpp::SaxParser::AttributeList::const_iterator iter = attributes.begin(); iter != attributes.end(); ++iter) {
		if(iter->name.compare("type")     == 0) type     = boost::lexical_cast<char>(iter->value.raw());
		if(iter->name.compare("end_time") == 0) end_time = timeToSec(iter->value.raw());
		if(iter->name.compare("duration") == 0) duration = boost::lexical_cast<int>(floor(boost::lexical_cast<float>(iter->value.raw())));
		if(iter->name.compare("node_id")  == 0) node_id  = boost::lexical_cast<int>(iter->value.raw());
	}

	int start_time = end_time - duration;
	while( start_time < 0 ) {
		start_time = start_time + 86400;
	}

	// determine the node id (transforming the original id)
	if( node_id != -1 ) {
		node_id = Data::getInstance()->getMapNodesOrigIdNewId().at(node_id);
	}

	// determine the house id
	if( node_id != -1 && type == 'm' ) {
		house_id = node_id;
	}
	if( node_id == -1 ) {
		node_id = house_id;
	}
	if( end_time == -1 ) {
		start_time = -1;
	}

	// creating the activity
	Activity cur_act(node_id, start_time, end_time, type);

	// adding it to the individual
	ind->addActivity(cur_act);

}

void VBSaxParser::on_end_element(const Glib::ustring& name) {
}

void VBSaxParser::on_characters(const Glib::ustring& text) {
}

void VBSaxParser::on_comment(const Glib::ustring& text) {
}

void VBSaxParser::on_warning(const Glib::ustring& text) {
}

void VBSaxParser::on_error(const Glib::ustring& text) {
}

void VBSaxParser::on_fatal_error(const Glib::ustring& text) {
}

