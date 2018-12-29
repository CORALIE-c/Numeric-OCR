/**
 * @file Color.hpp
 *
 * @todo Add description
 *
 * @author Matthew Rodusek (matthew.rodusek@gmail.com)
 * @date   September 26, 2015
 */

/*
 * Change Log:
 *
 * Sep 26, 2015: 
 * - Color.hpp created
 */
#ifndef RODU4140_COLOR_HPP_
#define RODU4140_COLOR_HPP_

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif

#include "base_types.hpp"
#include <ostream>

namespace rodu4140 {

  /////////////////////////////////////////////////////////////////////////////
  /// @class rodu4140::Color_RGB
  ///
  /// @brief The color object
  ///
  /////////////////////////////////////////////////////////////////////////////
  class Color_RGB{

    //-------------------------------------------------------------------------
    // Public Static Constants
    //-------------------------------------------------------------------------
  public:

    static const Color_RGB WHITE;
    static const Color_RGB CYAN;
    static const Color_RGB YELLOW;
    static const Color_RGB MAGENTA;
    static const Color_RGB RED;
    static const Color_RGB GREEN;
    static const Color_RGB BLUE;
    static const Color_RGB BLACK;
    static const Color_RGB GRAY;

    //-------------------------------------------------------------------------
    // Public Members
    //-------------------------------------------------------------------------
  public:

    ubyte r, ///< The red component of color
          g, ///< The green component of color
          b; ///< The blue component of color

    //-------------------------------------------------------------------------
    // Constructor / Assignment
    //-------------------------------------------------------------------------
  public:

    ///
    /// @brief Constructs a Color object without initializing the values
    ///
    Color_RGB();

    ///
    /// @brief Constructs a Color object
    ///
    /// @param r The red component of color
    /// @param g The green component of color
    /// @param b The blue component of color
    ///
    Color_RGB( ubyte r, ubyte g, ubyte b );

    ///
    /// @brief Constructs a Color object with a uniform (grayscale) color
    ///
    /// @param uniform the color to set uniform for all color values
    ///
    Color_RGB( ubyte uniform );

    ///
    /// @brief Constructs a Color object from another Color object
    ///
    /// @param other the other Color to copy
    ///
    Color_RGB( const Color_RGB& other );

    //-------------------------------------------------------------------------

    ///
    /// @brief Copies another color object
    ///
    /// @param other the Color object to copy
    /// @return pointer to (*this)
    ///
    Color_RGB& operator = ( const Color_RGB& other );

    ///
    /// @brief Sets the color as a uniform color
    ///
    /// @param uniform the uniform color to assign
    /// @return pointer to (*this)
    ///
    Color_RGB& operator = ( ubyte uniform );

    //-------------------------------------------------------------------------
    // Operators
    //-------------------------------------------------------------------------
  public:

    ///
    /// @brief Adds color values to this color
    ///
    /// @param rhs the color to add to this
    /// @return reference to (*this)
    ///
    Color_RGB& operator += ( const Color_RGB& rhs );

    ///
    /// @brief Subtracts color value from this color
    ///
    /// @param rhs the color to add to this
    /// @return reference to (*this)
    ///
    Color_RGB& operator -= ( const Color_RGB& rhs );

    ///
    /// @brief Multiply color by a scalar value
    ///
    /// @param rhs the scalar to multiply this color by
    /// @return reference to (*this)
    ///
    Color_RGB& operator *= ( float rhs );

    ///
    /// @brief Divide color by a scalar value
    ///
    /// @param rhs the scalar to divide by
    /// @return reference to (*this)
    ///
    Color_RGB& operator /= ( float rhs );

  };

  ///
  /// @brief Prints this color object to the specified ostream
  ///
  /// @param o   the ostream to print to
  /// @param rhs the color to print
  /// @return the ostream object
  ///
  std::ostream& operator << ( std::ostream& o, const Color_RGB& rhs );

  //---------------------------------------------------------------------------
  // Inline Definitions
  //---------------------------------------------------------------------------

  inline Color_RGB::Color_RGB(){
    // left empty -- for performance
  }

  inline Color_RGB operator + ( const Color_RGB& lhs, const Color_RGB& rhs ){
    return Color_RGB(lhs) += rhs;
  }

  inline Color_RGB operator - ( const Color_RGB& lhs, const Color_RGB& rhs ){
    return Color_RGB(lhs) -= rhs;
  }

  inline Color_RGB operator * ( const Color_RGB& lhs, float rhs ){
    return Color_RGB(lhs) *= rhs;
  }

  inline Color_RGB operator * ( float lhs, const Color_RGB& rhs ){
    return Color_RGB(rhs) *= lhs;
  }

  inline Color_RGB operator / ( const Color_RGB& lhs, float rhs ){
    return Color_RGB(lhs) /= rhs;
  }

}  // namespace rodu4140

#endif /* RODU4140_COLOR_HPP_ */
