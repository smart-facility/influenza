/****************************************************************
 * INDIVIDUAL.CPP
 * 
 * This file contains all the definitions of the methods of
 * individual.hpp (see that file for methods' documentation)
 * 
 * Authors: J. Barthelemy
 * Date   : 9 July 2015
 ****************************************************************/

#include "../include/Individual.hpp"

using namespace std;
using namespace repast;

IndividualPackage::IndividualPackage() :
		id(),
		init_proc(),
		agent_type(),
		cur_proc(),
		agenda(),
		cur_act(),
		age_cl(),
		gender(),
		socio_pro_status(),
		edu_level(),
		sick() {
}

IndividualPackage::IndividualPackage(int aId, int aInitProc, int aAgentType, int aCurProc, std::vector<Activity> aAgenda, int aCurAct, int aAgeCl,
									 char aGender, char aSocioProStatus, char aEduLevel, bool aSick) :
		id(aId),
		init_proc(aInitProc),
		agent_type(aAgentType),
		cur_proc(aCurProc),
		agenda(aAgenda),
		cur_act(aCurAct),
		age_cl(aAgeCl),
		gender(aGender),
		socio_pro_status(aSocioProStatus),
		edu_level(aEduLevel),
		sick(aSick) {
}

Individual::Individual(repast::AgentId id, std::vector<Activity> aAgenda, int aCurAct, int aAgeCl, char aGender, char aSocioProStatus, char aEduLevel, bool aSick) :
		_id(id),
		_agenda(aAgenda),
		_cur_act(aCurAct),
		_age_cl(aAgeCl),
		_gender(aGender),
		_socio_pro_status(aSocioProStatus),
		_edu_level(aEduLevel),
		_sick(aSick) {
}

Individual::Individual(repast::AgentId id, int aAgeCl, char aGender, char aSocioProStatus, char aEduLevel) :
		_id(id),
		_agenda(),
		_cur_act(0),
		_age_cl(aAgeCl),
		_gender(aGender),
		_socio_pro_status(aSocioProStatus),
		_edu_level(aEduLevel),
		_sick(false) {
}

Individual::Individual(repast::AgentId id, std::vector<Activity> aAgenda) :
		_id(id),
		_agenda(aAgenda),
		_cur_act(0),
		_age_cl(-1),
		_gender('X'),
		_socio_pro_status('X'),
		_edu_level('X'),
		_sick(false) {

}

Individual::~Individual() {
}

std::ostream& operator<<(std::ostream& out, const Individual &ind) {

	out << "Individual " << ind._id << endl;
	out << "  Gender: " << ind._gender << endl;
	out << "  Age class: " << ind._age_cl << endl;
	out << "  Socio-pro status: " << ind._socio_pro_status << endl;
	out << "  Education level: " << ind._edu_level << endl;
	out << "  Sick (1=yes, 0=no): " << ind._sick << endl;
	if( ind._agenda.size() > 0 ) {
		out << "  Activities:" << endl;
		for( auto &t : ind._agenda ) {
			out << t;
		}
	}

	return out;

}

int Individual::getTimeToNextActivity() const {

	if( _cur_act + 1 < (int)_agenda.size() ) {
		return _agenda[_cur_act+1].getEndTime();
	}
	return -1;

}

void Individual::addActivity(const Activity& aActivity) {

	_agenda.push_back(aActivity);

}

long Individual::getHouseNodeId() const {

	if( _agenda.size() == 0 ) {
		return -1;
	}
	return _agenda.back().getNodeId();

}

void Individual::print() const {
	cout << *this;
}

long Individual::getNodeId() const {
	return _agenda[_cur_act].getNodeId();
}

int Individual::getCurActStartingTime() const {
	return _agenda[_cur_act].getStartTime();
}


int Individual::getCurActEndTime() const {
	return _agenda[_cur_act].getEndTime();
}

bool Individual::setNextAct() {
	if( _cur_act < (int)_agenda.size() - 1 ) {
		_cur_act++;
		return true;
	}
	return false;
}

