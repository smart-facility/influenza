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
#include "repast_hpc/Point.h"
#include "repast_hpc/SharedDiscreteSpace.h"
#include "repast_hpc/GridComponents.h"
#include "repast_hpc/Moore2DGridQuery.h"

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

  Network                        _network;                      //!< road network

  // Aggregate outputs

  repast::SVDataSet*             _data_collection;              //!< aggregated output data set
  AggregateSum _total_susceptible;
  AggregateSum _total_latent;
  AggregateSum _total_infectious_asympt;
  AggregateSum _total_infectious_sympt;
  AggregateSum _total_recovered;
  AggregateSum _total_nodes_infected;

  // Model parameters

  float _r_beta;
  float _beta;
  float _epsilon;
  float _p_a;
  float _mu;
  float _max_inf;
  float _r_beta_x_beta;

  // Synch variables

  std::map<int, int>             _map_node_process;             //!< map containing identifying the process of every node
  std::map<repast::AgentId, int> _map_agents_to_move_process;   //!< map of the agents to be moved to other processes

  repast::SharedContext<Individual>* _agents;                    //!< shared context containing the individual agents of the simulation
  repast::SharedDiscreteSpace<Individual, repast::WrapAroundBorders, repast::SimpleAdder<Individual> >* _discrete_space; //!< spatial projection of the simulation.

 public :

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
  void providePackage(Individual* agent , std::vector<IndividualPackage>& out);

  //! Used by Repast HPC to create an Individual Agent from an IndividualPackage.
  /*!
    \param package a package containing an individual agent
    
    \return an individual agent
   */
  Individual* createAgent(IndividualPackage package);

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

  //! Check if a node belongs to the current process
  /*!
    \param nodeId the node to check

    \return true if the node belongs to the process, false otherwise
   */
  bool isInLocalBounds(int nodeId);

  //! Return the map associating the nodes id to their respective process
  /*!
    \return a map
   */
  const std::map<int, int>& getMapNodeProcess() const;

  //! Infect the agents
  void initInfectAgents();

  //! Infect the agents
  void initInfectAgents(std::vector<int> nodeIds, std::vector<int> nAgents, state_inf s);

  //! Adding an agent to the model SharedContext
  void addAgent(Individual* agent) {
	  _agents->addAgent(agent);
  }

  //! Move an agent to a given node
  void moveAgentToNode(repast::AgentId aId, int aNodeId) {
	  repast::Point<int> location(aNodeId, 0);
	  _discrete_space->moveTo(aId, location);
  }

  void gatherDataInd(const Individual& aInd);

  void resetDataInd();

};

#endif /* MODEL_HPP_ */
