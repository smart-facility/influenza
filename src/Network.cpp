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

Node::Node() : _id(-1), _x(0), _y(0), _infected(0) {

}

Node::Node(long id, float x, float y, int infected, vector<int> ind) :
          _id(id), _x(x), _y(y), _infected(infected), _ind(ind) {
}

Node::Node(long id, float x, float y) :
          _id(id), _x(x), _y(y), _infected(0), _ind() {
}

Network::Network() : _Nodes(), _n_infected_nodes(0) {
}

// Insert a node in the network
void Network::addNode(Node aNode) {

  _Nodes.insert(make_pair(aNode.getId(), aNode));

}
