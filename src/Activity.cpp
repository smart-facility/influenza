/*
 * Activity.cpp
 *
 *  Created on: 14 Aug 2015
 *      Author: johan
 */

#include "../include/Activity.hpp"

using namespace std;

Activity::Activity() : _node_id(), _start_time(), _end_time(), _type() {

}

Activity::Activity(long nodeId, int startTime, int endTime, char type) :
	_node_id(nodeId),
	_start_time(startTime),
	_end_time(endTime),
	_type(type) {
}

Activity::Activity(long nodeId) :
	_node_id(nodeId),
	_start_time(-1),
	_end_time(-1),
	_type('m') {

}

std::ostream& operator<<(std::ostream& out, const Activity &activity) {

	out << "  type: "       << activity._type       << endl;
	out << "    node id: "    << activity._node_id    << endl;
	out << "    start time: " << activity._start_time << endl;
	out << "    end time: "   << activity._end_time   << endl;

	return out;

}


