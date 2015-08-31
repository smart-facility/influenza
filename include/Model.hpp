/****************************************************************
 * MODEL.HPP
 *
 * This file contains all the VirtualBelgium's model and
 * scheduler related classes and functions.
 *
 * Authors: J. Barthelemy and L. Hollaert
 * Date   : 17 july 2012
 ****************************************************************/

/*! \file Model.hpp
 *  \brief VirtualBelgium's models and schedulers declarations.
 */

#ifndef MODEL_HPP_
#define MODEL_HPP_

#include "Individual.hpp"
#include "Data.hpp"
#include "tinyxml2.hpp"
#include "SaxParser.hpp"
#include "Network.hpp"

#include "repast_hpc/SharedContext.h"
#include "repast_hpc/Schedule.h"
#include "repast_hpc/Random.h"
#include "repast_hpc/Properties.h"
#include "repast_hpc/Utilities.h"
#include "repast_hpc/io.h"
#include "repast_hpc/logger.h"
#include "repast_hpc/initialize_random.h"
#include "repast_hpc/SVDataSetBuilder.h"

#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <stdexcept>
#include <cmath>
#include <vector>
#include <iomanip>
#include <map>
#include <boost/serialization/access.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/mpi.hpp>
#include <boost/mpi/collectives.hpp>
#include <boost/mpi/communicator.hpp>
#include <boost/unordered_set.hpp>
#include <boost/math/special_functions/pow.hpp>
#include <boost/range/algorithm.hpp>

//! Model class.
/*!
  This class contains the scheduler and is responsible for data aggregation.
  It is the core of TrafficSim.
 */
class Model {

 private :

  friend class ProviderReceiver;

  int                            _proc;                         //!< rank of the model's process
  repast::Properties&            _props;                        //!< properties of the model
  repast::SVDataSet*             _data_collection;              //!< aggregated output data set
  Network                        _network;                      //!< road network
  AggregateSum                   _total_agents;                 //!< number of agents remaining in the simulation
  AggregateSum                   _total_infected;               //!< total number of agents infected
  AggregateSum                   _total_nodes_infected;         //!< total number of nodes currently infected
  std::map<long, int>            _map_node_process;             //!< map containing identifying the process of every node
  std::map<repast::AgentId, int> _map_agents_to_move_process;   //!< map of the agents to be moved to other processes

 public :

  repast::SharedContext<Individual>* agents;               //!< Shared context containing the individual agents of the simulation

  //! Constructor.
  /*
   /param comm the mpi communicator
   /param props the model properties
   */
  Model( boost::mpi::communicator* comm, repast::Properties & props );

  //! Destructor.
  ~Model();
  

  //! Model agents initialization (MATSim input format).
  void init_agents_sax();

  //! Model agents localization initialization.
  void synch_agents();

  //! Initialization of the simulation's schedule.
  void initSchedule();

  //! Implements one step of the simulation.
  void step();

  //! Used by Repast HPC to exchange Individual agents between process.
  /*!
    \param agent the agent to exchange
    \param out the package containing the agent to exchange
   */
  void providePackage(Individual * agent , std::vector<IndividualPackage>& out);

  //! Used by Repast HPC to create an Individual Agent from an IndividualPackage.
  /*!
    \param package a package containing an individual agent
    
    \return an individual agent
   */
  Individual * createAgent(IndividualPackage package);

  //! Used by Repast HPC to packaged requested agents.
  /*!
    \param req the requested agents
    \param out the vector containing the packaged agents
   */
  void provideContent(repast::AgentRequest req, std::vector<IndividualPackage>& out);

  //! Update an individual given the information provided in the package.
  /*!
    \param package the package containing the new attributes values
   */
  void updateAgent(IndividualPackage package);

  //! Return the models properties read from properties file.
  const repast::Properties& getProps() const {
  	return _props;
  }

  //! Constructing the map of processes' nodes
  void constructMapNodeProcess();

  //! Check if the (x,y) coordinates belongs to the process local continuous space.
  /*!
    \param x a x coordinate
    \param y a y coordinate

    \return true if (x,y) in local continuous space, false otherwise
   */
  bool isInLocalBounds(long nodeId);

  const std::map<long, int>& getMapNodeProcess() const;

  void initInfectAgents(int n);

  void infectAgent(Individual *ind);

};

#endif /* MODEL_HPP_ */
