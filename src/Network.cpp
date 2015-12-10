/****************************************************************
 * NETWORK.CPP
 *
 * This file contains all the definitions of the methods of
 * network.hpp (see this file for methods' documentation)
 *
 * Authors: J. Barthelemy and L. Hollaert
 * Date   : 26 jun 2013
 ****************************************************************/

#include "../include/Network.hpp"

using namespace std;

// Constructor

Node::Node() : _id(-1), _infected(0) {

}

Node::Node(int id, int infected) :
          _id(id), _infected(infected) {
}

Node::Node(int id) :
          _id(id), _infected(0) {
}

Network::Network() : _Nodes(), _n_infected_nodes(0) {
}

// Insert a node in the network
void Network::addNode(Node aNode) {

  _Nodes.insert(make_pair(aNode.getId(), aNode));

}

void Network::dumpNodes() {

	ofstream f_out;
	f_out.open("dump_nodes", ios::out);

	for( auto n : _Nodes ) {

		f_out << n.second.getId() << endl;

	}
	f_out.close();

}
