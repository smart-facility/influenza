/****************************************************************
 * DATA.HPP
 * 
 * This file contains all the data related class and methods.
 * 
 * Authors: J. Barthelemy
 * Date   : 17 july 2015
 ****************************************************************/

/*! \file Data.hpp
 *  \brief Data related class and methods.
 */

#ifndef DATA_HPP_
#define DATA_HPP_

#include <repast_hpc/Properties.h>
#include <repast_hpc/RepastProcess.h>
#include <repast_hpc/TDataSource.h>
#include <repast_hpc/SVDataSet.h>
#include <iostream>
#include <sstream>
#include <map>
#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

#include "Network.hpp"
#include "tinyxml2.hpp"

//! \brief Singleton class for the Data class.
template <typename T>
class Singleton {

protected:

	//! Constructor.
	Singleton () {}

	//! Destructor.
	~Singleton () {}

public:

	//! Generates a singleton instance of a T object.
	/*!
    \param aProps model properties of VirtualBelgium
	 */
	static void makeInstance ( repast::Properties aProps ) {
		if (NULL == _singleton)  {
			_singleton = new T(aProps);
		}
	}

	//! Return the generated, unique, instance of a T object (if already instantiated).
	/*!
    \return the unique instance of a T object
	 */
	static T *getInstance () {
		if (NULL == _singleton)    {
			std::cerr << "No instance already created!" << std::endl;
		}
		return (static_cast<T*> (_singleton));
	}

	//! Free the memory.
	static void kill () {
		if (NULL != _singleton) {
			delete _singleton;
			_singleton = NULL;
		}
	}

private:

	static T *_singleton;    //!< unique instance of the Data class

};

//! Initialize the singleton to NULL.
template <typename T> T *Singleton<T>::_singleton = NULL;

//! \brief A data class.
/*!
 A data class reading and producing all the inputs required by
 TrafficSim. This class is implemented using a singleton design pattern.
 */
class Data : public Singleton<Data> {

	friend class Singleton<Data>;

private:

	Network            _network;           //!< network.
	repast::Properties _props;             //!< properties of simulation.

public:

	//! Constructor.
	/*!
      The constructor initialize all the private members of a Data object.

      \param aProps the properties of TrafficSim
	 */
	Data( repast::Properties aProps ) {

		if (repast::RepastProcess::instance()->rank() == 0) std::cout << "Data reading" << std::endl;

		// Models properties
		this->_props = aProps;

		// Network data
		read_network();

		std::ostringstream screen_output;
		screen_output << "INFO: Proc " << repast::RepastProcess::instance()->rank() << " parsed network data." << std::endl;
		screen_output << "INFO: Proc " << repast::RepastProcess::instance()->rank() << " has " << _network.getNodes().size() << " nodes." << std::endl;
		std::cout << screen_output.str();

	}

	//! Destructor.
	~Data() {};

	//! Read the road network (MATSim format).
	void read_network();

	//! Return the road network.
	/*!
      \return a road network
	 */
	const Network & getNetwork() const {
		return _network;
	}

};


//! \brief Aggregate output data class.
/*!
  This class is responsible for gathering aggregate data from the simulation.
 */
class AggregateSum : public repast::TDataSource<int> {

private :
	int _sum;   //!< the aggregate sum computed over all processes used to run the simulation

public :

	//! Constructor.
	AggregateSum();

	//! Destructor.
	~AggregateSum() {};

	//! Reset sum to 0.
	void setData(int val);

	//! Increment the data by 1.
	void incrementData();

	//! Decrement the data by 1.
	void decrementData();

	//! Return the current state of sum.
	/*!
    \return the value of sum
	 */
	int getData();

};


/////////////////////////////
//  Some useful routines.  //
/////////////////////////////


//! Convert a given time formatted as hh:mm:ss to the number of seconds since midnight.
/*!
  \param aTime a string that represents an hour in the hh:mm:ss format

  \return the number of seconds elapsed since midnight
 */
long timeToSec( const std::string &aTime );


//! Decompose a string according to a separator into a vector of type T.
/*!
 \param msg the string to decompose
 \param separators separators used for the decomposition

 \return a vector of type T
 */
template<typename T>
std::vector<T> split(const std::string & msg, const std::string & separators);


#endif /* DATA_HPP_ */
