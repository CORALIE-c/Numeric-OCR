/**
 * @file Color.cpp
 *
 * @todo Add description
 *
 * @author Matthew Rodusek (matthew.rodusek@gmail.com)
 * @date   Sep 26, 2015
 *
 */

/*
 * Change Log:
 *
 * Sep 26, 2015: 
 * - Color.cpp created
 */
#include "Color.hpp"

namespace rodu4140 {

  //-------------------------------------------------------------------------
  // Public Static Constants
  //-------------------------------------------------------------------------

  const Color_RGB Color_RGB::WHITE( 255, 255, 255 );
  const Color_RGB Color_RGB::CYAN( 0, 255, 255 );
  const Color_RGB Color_RGB::YELLOW( 255, 255, 0 );
  const Color_RGB Color_RGB::MAGENTA( 255, 0, 255 );
  const Color_RGB Color_RGB::RED( 255, 0, 255 );
  const Color_RGB Color_RGB::GREEN( 0, 255, 0 );
  const Color_RGB Color_RGB::BLUE( 0, 0, 255 );
  const Color_RGB Color_RGB::BLACK( 0, 0, 0 );
  const Color_RGB Color_RGB::GRAY( 128, 128, 128 );

  //---------------------------------------------------------------------------
  // Constructor / Assignment
  //---------------------------------------------------------------------------

  Color_RGB::Color_RGB( ubyte r, ubyte g, ubyte b )
    :  r(r),
      g(g),
      b(b)
  {

  }

  Color_RGB::Color_RGB( ubyte uniform )
    : r(uniform),
      g(uniform),
      b(uniform)
  {

  }

  Color_RGB::Color_RGB( const Color_RGB& other )
    :  r(other.r),
      g(other.g),
      b(other.b)
  {

  }

  //---------------------------------------------------------------------------

  Color_RGB& Color_RGB::operator = ( const Color_RGB& other ){
    this->r = other.r;
    this->g = other.g;
    this->b = other.b;
    return (*this);
  }

  Color_RGB& Color_RGB::operator = ( ubyte uniform ){
    this->r = uniform;
    this->g = uniform;
    this->b = uniform;
    return (*this);
  }

  //---------------------------------------------------------------------------
  // Operators
  //---------------------------------------------------------------------------

  Color_RGB& Color_RGB::operator += ( const Color_RGB& rhs ){
    this->r += rhs.r;
    this->g += rhs.g;
    this->b += rhs.b;
    return (*this);
  }

  Color_RGB& Color_RGB::operator -= ( const Color_RGB& rhs ){
    this->r -= rhs.r;
    this->g -= rhs.g;
    this->b -= rhs.b;
    return (*this);
  }

  //---------------------------------------------------------------------------

  Color_RGB& Color_RGB::operator *= ( float rhs ){
    this->r *= rhs;
    this->g *= rhs;
    this->b *= rhs;
    return (*this);
  }

  Color_RGB& Color_RGB::operator /= ( float rhs ){
    this->r /= rhs;
    this->g /= rhs;
    this->b /= rhs;
    return (*this);
  }

  //---------------------------------------------------------------------------

  std::ostream& operator << ( std::ostream& o, const Color_RGB& rhs ){
    o << "rgb("
      << (int) rhs.r << ","
      << (int) rhs.g << ","
      << (int) rhs.b << ")";

    return o;
  }

}  // namespace rodu4140

