/****************************************************************
 * DATA.CPP
 * 
 * This file contains all the definitions of the methods of
 * Data.hpp (see this file for methods' documentation)
 * 
 * Authors: J. Barthélemy
 * Date   : 3 June 2014
 ****************************************************************/

#include "../include/Data.hpp"

using namespace std;
using namespace repast;
using namespace tinyxml2;


////////////////
// Data class //
////////////////


void Data::read_network() {

	// Number of procs in the simulation
	int n_proc = RepastProcess::instance()->worldSize();
	int cur_proc = RepastProcess::instance()->rank();

	// Loading XML file containing the network data.
	string filename = this->_props.getProperty("file.network");
	XMLDocument doc(filename.c_str());
	doc.loadFile(filename.c_str());

	// Parsing the node data ////////////////////////////////////////////////////////////////

	if (cur_proc == 0) cout << "... reading network from " << filename.c_str() << endl;

	XMLElement * ele = doc.FirstChildElement("network")->FirstChildElement("nodes")->FirstChildElement("node");
	const XMLAttribute * attr;

	int i = 0;
	while (ele) {

		// reading id
		attr = ele->FirstAttribute();
		int id = attr->IntValue();
		this->_map_nodes_orig_id_new_id[id] = i;
		this->_map_nodes_new_id_orig_id[i]  = id;

		// only add the nodes of the current process
		if ( i % n_proc == cur_proc ) {
			Node currNode(i);
			this->_network.addNode(currNode);
		}

		ele = ele->NextSiblingElement("node");
		i++;

	}

	if ( cur_proc == 0 ) cout << "INFO: DATA GENERATION: Number total of nodes read " << i << endl;

	if (cur_proc == 0) cout << "... done! " << endl;

}


/////////////////////////////////
// Aggregate output data class //
/////////////////////////////////


AggregateSum::AggregateSum() {
	_sum = 0;
}

void AggregateSum::setData(int val) {
	_sum = val;
}

void AggregateSum::incrementData() {
	++_sum;
}

void AggregateSum::decrementData() {
	--_sum;
}

int AggregateSum::getData() {
	return _sum;
}

void AggregateSum::resetData() {
	_sum = 0;
}


///////////////////////
// Some useful tools //
///////////////////////


long timeToSec(const std::string &aTime ) {

	vector<long> time = split<long>(aTime, ":");
	return time[0] * 3600 + time[1] * 60 + time[2];

}
