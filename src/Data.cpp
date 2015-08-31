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

	long int i = 1;
	while (ele) {

		// only add the nodes of the current process
		if ( i % n_proc == cur_proc ) {

			// reading id
			attr = ele->FirstAttribute();
			long id = attr->LongValue();

			// reading x coordinate
			attr = attr->Next();
			float x = attr->FloatValue();

			// reading y coordinate
			attr = attr->Next();
			float y = attr->FloatValue();

			// adding the node to the network
			Node currNode(id, x, y);
			this->_network.addNode(currNode);

		}

		ele = ele->NextSiblingElement("node");
		i++;

	}

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


///////////////////////
// Some useful tools //
///////////////////////


long timeToSec( const std::string &aTime ) {

	std::vector<long> time = split<long>(aTime, ":");
	return time[0] * 3600 + time[1] * 60 + time[2];

}

template<typename T>
std::vector<T> split(const std::string & msg, const std::string & separators) {

	std::vector<T> result;                                         // resulting vector
	T              token;                                          // one token of the string
	boost::char_separator<char> sep(separators.c_str());           // separator
	boost::tokenizer<boost::char_separator<char> > tok(msg, sep);  // token's generation

	// string decomposition into token
	for (boost::tokenizer<boost::char_separator<char> >::const_iterator i = tok.begin(); i != tok.end(); i++) {

		std::stringstream s(*i);
		string s_string = s.str();                                   // getting the string
	    boost::algorithm::trim(s_string);                            // removing trailing blanks
	    token = boost::lexical_cast<T>( s_string );                  // casting operator

		result.push_back(token);
	}

	// returning the resulting decomposition
	return result;
}
