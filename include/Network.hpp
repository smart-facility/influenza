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

//! A node class.
/*!
 This class intends to represents the nodes of a road network.
 */
class Node {

private:

  long             _id;            //!< id of the node
  float            _x;             //!< x coordinate
  float            _y;             //!< y coordinate
  int              _infected;      //!< number of infected individuals on the node
  std::vector<int> _ind;           //!< list of the individual on the node

public:

  //! Default Constructor.
  Node();

  //! Constructor.
  /*!
    \param id a node id
    \param x x coordinate
    \param y y coordinate
   */
  Node(long id, float x, float y, int infected, std::vector<int> ind);
  Node(long id, float x, float y);

  //! Destructor.
  ~Node() {
  }
  long getId() const {
	  return _id;
  }

  void setId(long id) {
	  _id = id;
  }

  const std::vector<int>& getInd() const {
	  return _ind;
  }

  void setInd(const std::vector<int>& ind) {
	  _ind = ind;
  }

  int getInfected() const {
	  return _infected;
  }

  void setInfected(int infected) {
	  _infected = infected;
  }

  float getX() const {
	  return _x;
  }

  void setX(float x) {
	  _x = x;
  }

  float getY() const {
	  return _y;
  }

  void setY(float y) {
	  _y = y;
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

  std::map<long, Node> _Nodes;    //!< Nodes of the network (see Node class)
  long _n_infected_nodes;         //!< Number of infected nodes in the network

public:

  //! Constructor.
  Network();

  //! Destructor.
  ~Network() {
  }

  const std::map<long, Node>& getNodes() const {
	  return _Nodes;
  }

  void setNodes(const std::map<long, Node>& nodes) {
	  _Nodes = nodes;
  }

  bool isNodeInfected(long aNodeId) {
	  if(_Nodes.at(aNodeId).getInfected() > 0) return true;
	  return false;
  }

  void addNode(Node aNode);

  void addInfectedNode(long aNodeId) {
	   if (_Nodes.at(aNodeId).addInfected() == 1) {
		   _n_infected_nodes++;
	   }
  }

  void removeInfectedNode(long aNodeId) {
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

};

#endif /* NETWORK_HPP_ */
