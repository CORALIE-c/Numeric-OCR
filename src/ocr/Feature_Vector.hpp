/**
 * @file Feature.hpp
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
 * - Feature.hpp created
 */
#ifndef OCR_FEATURE_VECTOR_HPP_
#define OCR_FEATURE_VECTOR_HPP_

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif

#include "base_types.hpp"

#include <vector>  // std::vector
#include <iosfwd>  // std::ostream decl
#include <cstddef> // std::size_t

namespace ocr {

  class Feature_Vector  {

    //-----------------------------------------------------------------------
    // Public Member Types
    //-----------------------------------------------------------------------
  public:

    typedef std::vector<double> feature_collection;

    typedef typename std::vector<double>::iterator       iterator;
    typedef typename std::vector<double>::const_iterator const_iterator;

    //-----------------------------------------------------------------------
    // Constructor
    //-----------------------------------------------------------------------
  public:

    Feature_Vector( feature_collection& features );

    //-----------------------------------------------------------------------
    // Capacity
    //-----------------------------------------------------------------------
  public:

    std::size_t size() const;
    double magnitude() const;

    //-----------------------------------------------------------------------
    // Access
    //-----------------------------------------------------------------------
  public:

    double& at(std::size_t i);
    double at(std::size_t i) const;
    double& operator[](std::size_t i);
    double operator[](std::size_t i) const;

    //-----------------------------------------------------------------------
    // Operators
    //-----------------------------------------------------------------------
  public:

    Feature_Vector& operator -= ( const Feature_Vector& rhs );

    //-----------------------------------------------------------------------
    // Iterators
    //-----------------------------------------------------------------------
  public:

    iterator begin();
    iterator end();

    const_iterator begin() const;
    const_iterator end() const;

    const_iterator cbegin() const;
    const_iterator cend() const;

    //-----------------------------------------------------------------------
    // Private Members
    //-----------------------------------------------------------------------
  private:

    feature_collection m_features; ///< Collection of features

  };

  std::ostream& operator << ( std::ostream& o, const Feature_Vector& rhs );

  //--------------------------------------------------------------------------
  // Access
  //--------------------------------------------------------------------------

  inline double& Feature_Vector::operator[](std::size_t i){
    return at(i);
  }

  inline double Feature_Vector::operator[](std::size_t i) const{
    return at(i);
  }

  //--------------------------------------------------------------------------
  // Operators
  //--------------------------------------------------------------------------

  inline Feature_Vector operator - ( const Feature_Vector& lhs, const Feature_Vector& rhs ){
    return Feature_Vector(lhs) -= rhs;
  }

  //--------------------------------------------------------------------------
  // Comparisons
  //--------------------------------------------------------------------------

  inline bool operator ==( const Feature_Vector& lhs, const Feature_Vector& rhs ){
    return lhs.magnitude() == rhs.magnitude();
  }

  inline bool operator !=( const Feature_Vector& lhs, const Feature_Vector& rhs ){
    return !(lhs == rhs);
  }

  inline bool operator < ( const Feature_Vector& lhs, const Feature_Vector& rhs ){
    return lhs.magnitude() < rhs.magnitude();
  }

  inline bool operator > ( const Feature_Vector& lhs, const Feature_Vector& rhs ){
    return rhs < lhs;
  }

  inline bool operator <=( const Feature_Vector& lhs, const Feature_Vector& rhs ){
    return !(rhs < lhs);
  }

  inline bool operator >=( const Feature_Vector& lhs, const Feature_Vector& rhs ){
    return !(lhs < rhs);
  }

}  // namespace ocr

#endif /* OCR_FEATURE_VECTOR_HPP_ */
