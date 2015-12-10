/****************************************************************
 * NETWORK.HPP
 *
 * This file contains all the data related class and methods.
 *
 * Authors: J. Barthelemy
 * Date   : 08 april 2013
 ****************************************************************/

/*! \file Network.hpp
    \brief Road network related class and methods.
 */

#ifndef NETWORK_HPP_
#define NETWORK_HPP_

#include <vector>
#include <map>
#include <iostream>
#include <fstream>

//! A node class.
/*!
 This class intends to represents the nodes of a road network.
 */
class Node {

private:

  int _id;            //!< id of the node
  int _infected;      //!< number of infected individuals on the node

public:

  //! Default Constructor.
  Node();

  //! Constructor.
  /*!
    \param id a node id
    \param x x coordinate
    \param y y coordinate
   */
  Node(int id, int infected);
  Node(int id);

  //! Destructor.
  ~Node() {
  }
  int getId() const {
	  return _id;
  }

  void setId(int id) {
	  _id = id;
  }

  int getInfected() const {
	  return _infected;
  }

  void setInfected(int infected) {
	  _infected = infected;
  }


  int addInfected() {
	  return ++_infected;
  }

  int removeInfected() {
	  return --_infected;
  }

};

class Network {

private:

  std::map<int, Node> _Nodes;    //!< Nodes of the network (see Node class)
  long _n_infected_nodes;         //!< Number of infected nodes in the network

public:

  //! Constructor.
  Network();

  //! Destructor.
  ~Network() {
  }

  const std::map<int, Node>& getNodes() const {
	  return _Nodes;
  }

  void setNodes(const std::map<int, Node>& nodes) {
	  _Nodes = nodes;
  }

  bool isNodeInfected(int aNodeId) {
	  if(_Nodes.at(aNodeId).getInfected() > 0) return true;
	  return false;
  }

  void addNode(Node aNode);

  void addInfectedNode(int aNodeId) {
	   if (_Nodes.at(aNodeId).addInfected() == 1) {
		   _n_infected_nodes++;
	   }
  }

  void removeInfectedNode(int aNodeId) {
	  if (_Nodes.at(aNodeId).removeInfected() == 0 ) {
		  _n_infected_nodes--;
	  }
  }

  long getNInfectedNodes() const {
	  return _n_infected_nodes;
  }

  void setNInfectedNodes(long infectedNodes) {
	  _n_infected_nodes = infectedNodes;
  }

  void dumpNodes();

};

#endif /* NETWORK_HPP_ */
