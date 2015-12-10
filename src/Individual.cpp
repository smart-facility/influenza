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

ostream& operator<<(ostream& out, const state_inf &state) {

	switch(state) {
		case state_inf::SUSCEPTIBLE:       return out << "Susceptible";
		case state_inf::LATENT:            return out << "Latent";
		case state_inf::INFECTIOUS_SYMPT:  return out << "Infectious symptomatic";
		case state_inf::INFECTIOUS_ASYMPT: return out << "Infectious asymptomatic";
		case state_inf::RECOVERED:         return out << "Recovered";
		default:                           return out << "Unknown state of infection!";
	}

}

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
		state(state_inf::SUSCEPTIBLE),
		time_next_state() {
}

IndividualPackage::IndividualPackage(int aId, int aInitProc, int aAgentType, int aCurProc, std::vector<Activity> aAgenda, int aCurAct, int aAgeCl,
									 char aGender, char aSocioProStatus, char aEduLevel, state_inf aState, int aTime) :
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
		state(aState),
		time_next_state(aTime) {
}

Individual::Individual(repast::AgentId id, std::vector<Activity> aAgenda, int aCurAct, int aAgeCl, char aGender, char aSocioProStatus,
					   char aEduLevel, state_inf aState, int aTime) :
		_id(id),
		_agenda(aAgenda),
		_cur_act(aCurAct),
		_age_cl(aAgeCl),
		_gender(aGender),
		_socio_pro_status(aSocioProStatus),
		_edu_level(aEduLevel),
		_state(aState),
		_time_next_state(aTime) {
}

Individual::Individual(repast::AgentId id, int aAgeCl, char aGender, char aSocioProStatus, char aEduLevel) :
		_id(id),
		_agenda(),
		_cur_act(0),
		_age_cl(aAgeCl),
		_gender(aGender),
		_socio_pro_status(aSocioProStatus),
		_edu_level(aEduLevel),
		_state(state_inf::SUSCEPTIBLE),
        _time_next_state(0) {
}

Individual::Individual(repast::AgentId id, std::vector<Activity> aAgenda) :
		_id(id),
		_agenda(aAgenda),
		_cur_act(0),
		_age_cl(-1),
		_gender('X'),
		_socio_pro_status('X'),
		_edu_level('X'),
		_state(state_inf::SUSCEPTIBLE),
		_time_next_state(0) {

}

Individual::~Individual() {
}

std::ostream& operator<<(std::ostream& out, const Individual &ind) {

	out << "Individual " << ind._id << endl;
	out << "  Gender: " << ind._gender << endl;
	out << "  Age class: " << ind._age_cl << endl;
	out << "  Socio-pro status: " << ind._socio_pro_status << endl;
	out << "  Education level: " << ind._edu_level << endl;
	out << "  State: " << ind._state << endl;
	out << "  Transition time: " << ind._time_next_state << endl;
	if( ind._agenda.size() > 0 ) {
		out << "  Activities:" << endl;
		for( auto &t : ind._agenda ) {
			out << t;
		}
	}

	return out;

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

long Individual::getCurActNodeId() const {
	return _agenda[_cur_act].getNodeId();
}

int Individual::getCurActStartingTime() const {
	return _agenda[_cur_act].getStartTime();
}


int Individual::getCurActEndTime() const {
	return _agenda[_cur_act].getEndTime();
}

bool Individual::setNextAct() {
/*
	if( _id.id() == 10042230 ) {
		cout << "DEBUG: AGENT " << _id.id() << endl;
		cout << "      ag size " << _agenda.size() << endl;
		cout << "      cur act " << _cur_act << endl;
	}
*/
	if( _cur_act < (int)_agenda.size() - 2 ) {
		_cur_act++;
/*
		if( _id.id() == 10042230 ) {
			cout << " NEW SELECTECD ACT " << _cur_act << endl;
			cout << "     end time :" << _agenda[_cur_act].getEndTime() << endl;
		}
		*/
		return true;
	}

	_cur_act = 0;
	/*
	if( _id.id() == 10042230 ) {
		cout << " RESET SCHEDULE " << endl;
		cout << " NEW SELECTECD ACT " << _cur_act << endl;
		cout << "     end time :" << _agenda[_cur_act].getEndTime() << endl;
	}
*/

	return false;
}

bool Individual::isLatent( float aInfectionProba ) {

	float p = (float)Random::instance()->nextDouble();

	// agent become latent
	if( p < aInfectionProba ) {
		// ... time before becoming infectious
		double temp = Random::instance()->getGenerator("epsilon")->next() * 86400;
		_time_next_state = (int)temp;
		_state = state_inf::LATENT;
		return true;
	}

	return false;

}

void Individual::decreaseTimeTransition() {

	if ( _time_next_state > 0 ) {
		_time_next_state--;
	}

}

void Individual::determineInfectiousType( float aAsymptomicInfectiousProba ) {

	// selection of the infection type: symptomic or asymptomatic
	float p = (float)Random::instance()->nextDouble();
	if( p < aAsymptomicInfectiousProba ) {
		_state = state_inf::INFECTIOUS_ASYMPT;
	} else {
		_state = state_inf::INFECTIOUS_SYMPT;
	}

	// time to recover
	double temp = Random::instance()->getGenerator("mu")->next() * 86400;
	_time_next_state  = (int)temp;


}

// Reset the individual schedule and select the appropriate activity given the current time of the day
bool Individual::resetSchedule( int aTime ) {

	bool is_active = false;

	// reset activity
	_cur_act = 0;
	//while( _agenda[_cur_act].getEndTime() < aTime ) {
	//	_cur_act++;
	//}

	//cout << "DEBUG: RESETSCHEDULE, agent " << _id.id() << " SELECTED ACT " << _cur_act << " AT TIME " << aTime << endl;

	// check if individual is performing an activity
	if( _agenda[_cur_act].getStartTime() < aTime ) {
		is_active = true;
		//cout << "      AGT IS ACTIVE!" << endl;
	}

	return is_active;

}
