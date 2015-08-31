/*
 * Activity.hpp
 *
 *  Created on: 08/08/2014
 *      Author: johan
 */

#ifndef ACTIVITY_HPP_
#define ACTIVITY_HPP_

#include <boost/serialization/access.hpp>
#include <iostream>

class Activity {

	friend class boost::serialization::access;

private:

	template<class Archive>
	void serialize(Archive & ar, const unsigned int version ) {
		  ar &_node_id;
		  ar &_start_time;
		  ar &_end_time;
		  ar &_type;
	  }

	long _node_id;    //!< id of the node where the activity takes place
	int  _start_time; //!< starting time of the activity
	int  _end_time;   //!< duration of the activity
	char _type;       //!< type of the activity

public:

	//! Defaut constructor.
	Activity();

	//! Complete constructor.
	Activity(long nodeId, int startTime, int endTime, char type);

	Activity(long nodeId);

	//! Destructor.
	~Activity() {};

	friend std::ostream& operator<<(std::ostream& out, const Activity &activity);

	int getEndTime() const {
		return _end_time;
	}

	void setEndTime(int endTime) {
		_end_time = endTime;
	}

	long getNodeId() const {
		return _node_id;
	}

	void setNodeId(long nodeId) {
		_node_id = nodeId;
	}

	int getStartTime() const {
		return _start_time;
	}

	void setStartTime(int startTime) {
		_start_time = startTime;
	}

	char getType() const {
		return _type;
	}

	void setType(char type) {
		_type = type;
	}

};

#endif /* ACTIVITY_HPP_ */
