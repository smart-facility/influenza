/****************************************************************
 * INDIVIDUAL.HPP
 * 
 * This file contains all the Individuals related classes and 
 * functions.
 * 
 * Authors: J. Barthelemy
 * Date   : 9 July 2012
 ****************************************************************/

/*! \file Individual.hpp
 *  \brief Individual agents related classes and methods declarations.
 */


#ifndef INDIVIDUAL_HPP_
#define INDIVIDUAL_HPP_

#include <boost/serialization/access.hpp>
#include <boost/lexical_cast.hpp>
#include <vector>
#include <iostream>
#include "repast_hpc/AgentId.h"
#include "repast_hpc/Random.h"
#include "Activity.hpp"

const int MODEL_AGENT_IND_TYPE = 0;     //!< constant for the individual agent type

enum class state_inf : unsigned int { SUSCEPTIBLE = 1, LATENT = 2, INFECTIOUS_SYMPT = 3, INFECTIOUS_ASYMPT = 4, RECOVERED = 5 };
std::ostream& operator<<(std::ostream& out, const state_inf &state);


//! \brief The package structure for Individual agents.
/*!
  A structure used for passing individual agents from one process to another.
 */
struct IndividualPackage {

	friend class boost::serialization::access;

	int                   id;                 //!< Id of the individual.
	int                   init_proc;          //!< Initial individual process.
	int                   agent_type;         //!< Individual agent_type
	int                   cur_proc;           //!< Current individual process
	std::vector<Activity> agenda;             //!< Individual's agenda.
	int                   cur_act;            //!< Current id of the activity
	int                   age_cl;             //!< Individual's age class.
	char                  gender;             //!< Individual's gender.
	char                  socio_pro_status;   //!< Individual's socio-professional status.
	char                  edu_level;          //!< Individual's education level.
	state_inf             state;              //!< Individual's sickness status.
	int                   time_next_state;    //!< Individual's time before next state transition.

	//! Default constructor.
	IndividualPackage();
	//! Complete constructor.
	IndividualPackage(int aId, int aInitProc, int aAgentType, int aCurProc, std::vector<Activity> aAgenda, int aCurAct ,int aAgeCl,
			char aGender, char aSocioProStatus, char aEduLevel, state_inf aState, int aTime);

	//! Serializing procedure of the package.
	/*!
    \param ar an Archive in which the individual will be packaged
    \param version version of the package
	 */
	template <class Archive>
	void serialize ( Archive &ar , const unsigned int version ) {
		ar &id;
		ar &init_proc;
		ar &agent_type;
		ar &cur_proc;
		ar &agenda;
		ar &cur_act;
		ar &age_cl;
		ar &gender;
		ar &socio_pro_status;
		ar &edu_level;
		ar &state;
		ar &time_next_state;
	};

};

//! The individual agent class
/*!
  This class implements the individual agents in TrafficSim.
  Each individual is characterized by
  - an individual id of the type repast::AgentId;
  - an vector of activities to carry;
  - a gender;
  - a socio-professional status;
  - an education level;
  - being sick or not.
 */
class Individual : public repast::Agent {

	// Enabling serialization
	friend class boost::serialization::access;

private :

	repast::AgentId       _id;                 //!< Individual's Repast::AgentId.
	std::vector<Activity> _agenda;             //!< Individual's agenda.
	int                   _cur_act;            //!< Position of the current activity in the agenda
	int                   _age_cl;             //!< Individual's age class.
	char                  _gender;             //!< Individual's gender.
	char                  _socio_pro_status;   //!< Individual's socio-professional status.
	char                  _edu_level;          //!< Individual's education level.
	state_inf             _state;              //!< Individual's sickness status.
	int                   _time_next_state;    //!< Individual's time before becoming infectious.

public :

	//! Constructors.
	Individual();
	Individual(repast::AgentId id, std::vector<Activity> aAgenda, int aCurAct, int aAgeCl, char aGender, char aSocioProStatus, char aEduLevel, state_inf aState, int aTime);
	Individual(repast::AgentId id, int aAgeCl, char aGender, char aSocioProStatus, char aEduLevel);
	Individual(repast::AgentId id, std::vector<Activity> aAgenda);

	//! Destructor.
	virtual ~Individual();

	//! Return the individual Repast agent id (required by Repast).
	/*!
    \return the individual Repast agent id
	 */
	repast::AgentId & getId() {
		return _id ;
	}

	//! Return the individual Repast agent id (required by Repast).
	/*!
    \return the individual Repast agent id
	 */
	const repast::AgentId & getId() const {
		return _id;
	}

	int getAgeCl() const {
		return _age_cl;
	}

	void setAgeCl(unsigned int ageCl) {
		_age_cl = ageCl;
	}

	int getCurAct() const {
		return _cur_act;
	}

	void setCurAct(int curAct) {
		_cur_act = curAct;
	}

	const std::vector<Activity>& getAgenda() const {
		return _agenda;
	}

	void setAgenda(const std::vector<Activity>& agenda) {
		_agenda = agenda;
	}

	char getEduLevel() const {
		return _edu_level;
	}

	void setEduLevel(char eduLevel) {
		_edu_level = eduLevel;
	}

	char getGender() const {
		return _gender;
	}

	void setGender(char gender) {
		_gender = gender;
	}

	state_inf getState() const {
		return _state;
	}

	void setState(state_inf state) {
		_state = state;
	}

	char getSocioProStatus() const {
		return _socio_pro_status;
	}

	void setSocioProStatus(char socioProStatus) {
		_socio_pro_status = socioProStatus;
	}

	int getTimeTransition() const {
		return _time_next_state;
	}

	void setTimeTransition(int aTime) {
		_time_next_state = aTime;
	}

	long getHouseNodeId() const;

	long getCurActNodeId() const;

	void addActivity(const Activity& aActivity);

	void print() const;

	int getCurActStartingTime() const;

	int getCurActEndTime() const;

	// try to increment the current activity counter and return true if it was possible, false otherwise
	bool setNextAct();

	//! Overloading << operator.
	friend std::ostream& operator<<(std::ostream& out, const Individual &ind);

	bool isLatent( float aInfectionProba );

	void decreaseTimeTransition();

	void determineInfectiousType( float aInfectionTypeProba );

	bool resetSchedule( int aTime );

};

#endif /* INDIVIDUAL_HPP_ */
