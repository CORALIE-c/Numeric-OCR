/**
 * @file Feature.cpp
 *
 * @todo Add description
 *
 * @author Matthew Rodusek (matthew.rodusek@gmail.com)
 * @date   November 16, 2015
 *
 */

/*
 * Change Log:
 *
 * Nov 16, 2015: 
 * - Feature.cpp created
 */
#include "Feature_Vector.hpp"

namespace rodu4140 {

	//--------------------------------------------------------------------------
	// Constructor
	//--------------------------------------------------------------------------

	Feature_Vector::Feature_Vector( feature_collection& features )
		: m_features(features) // Copy all features from vector
	{

	}

	//--------------------------------------------------------------------------
	// Capacity
	//--------------------------------------------------------------------------

	size_t Feature_Vector::size() const{
		return m_features.size();
	}

	double Feature_Vector::magnitude() const{
		double sum = 0.0;
		feature_collection::const_iterator iter = m_features.begin();

		for( ; iter != m_features.end(); ++iter ){
			sum += (*iter)*(*iter);
		}
		return sum;
	}

	//--------------------------------------------------------------------------
	// Access
	//--------------------------------------------------------------------------

	double& Feature_Vector::at(size_t i){
		return m_features[i];
	}

	double Feature_Vector::at(size_t i) const{
		return m_features[i];
	}

	//--------------------------------------------------------------------------
	// Operator
	//--------------------------------------------------------------------------

	Feature_Vector& Feature_Vector::operator -=( const Feature_Vector& rhs ){
		feature_collection::const_iterator iter = rhs.begin();
		feature_collection::iterator cur        = m_features.begin();

		for( ; iter != m_features.end() && cur != this->end(); ++iter, ++cur ){
			(*cur) -= (*iter);
		}
		return (*this);
	}

	//--------------------------------------------------------------------------
	// Iterators
	//--------------------------------------------------------------------------

	Feature_Vector::iterator Feature_Vector::begin(){
		return m_features.begin();
	}

	Feature_Vector::iterator Feature_Vector::end(){
		return m_features.end();
	}

	//--------------------------------------------------------------------------

	Feature_Vector::const_iterator Feature_Vector::begin() const{
		return m_features.begin();
	}

	Feature_Vector::const_iterator Feature_Vector::end() const{
		return m_features.end();
	}

	//--------------------------------------------------------------------------

	Feature_Vector::const_iterator Feature_Vector::cbegin() const{
		return m_features.cbegin();
	}

	Feature_Vector::const_iterator Feature_Vector::cend() const{
		return m_features.cend();
	}

	std::ostream& operator << ( std::ostream& o, const Feature_Vector& rhs ){
		o << "[ ";
		for( Feature_Vector::const_iterator iter = rhs.begin(); iter != rhs.end(); ++iter ){
			o << *iter << ", ";
		}
		o << "]";

		return o;
	}


}  // namespace rodu4140

