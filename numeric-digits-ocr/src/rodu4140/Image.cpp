/**
 * @file Image.cpp
 *
 * @todo Add description
 *
 * @author Matthew Rodusek (matthew.rodusek@gmail.com)
 * @date   Sep 23, 2015
 *
 */

/*
 * Change Log:
 *
 * Sep 23, 2015: 
 * - Image.cpp created
 */
#include "Image.hpp"

#include <cstring>
#include <utility>

namespace rodu4140{

	//---------------------------------------------------------------------------
	// Constructor / Destructor
	//---------------------------------------------------------------------------

	Image::Image( size_type width, size_type height )
		:	m_width(width),
			m_height(height),
			m_data( new pixel_type[width * height] )
	{

	}

	Image::Image( size_type width, size_type height, pixel_type* data )
		:	m_width(width),
			m_height(height),
			m_data(data)
	{

	}

	Image::Image( const Image& x )
		:	m_width(x.width()),
			m_height(x.height()),
			m_data(new pixel_type[m_width*m_height])
	{
		std::copy( x.m_data, x.m_data + m_width*m_height, m_data );
	}

#ifdef RODU4140_COMPILER_HAS_CPP11_RVALUE

	Image::Image( Image&& x )
		:	m_width(std::move(x.m_width)),
			m_height(std::move(x.m_height)),
			m_data(std::move(x.m_data))
	{

	}

#endif

	//---------------------------------------------------------------------------

	Image::~Image(){
		if(m_data){
			delete [] m_data;
		}
	}

	//---------------------------------------------------------------------------
	// Element Access
	//---------------------------------------------------------------------------

	Image::pixel_type Image::at( int x, int y ) const{
		if( x < 0 || x >= (int) m_width || y < 0 || y >= (int) m_height ){

			return pixel_type(255,255,255);
		}else{
			pixel_type* pixel = m_data + ((y * m_width) + x);

			return *pixel;
		}
	}

	ubyte Image::at_grayscale( int x, int y ) const{
		if( x < 0 || x >= (int) m_width || y < 0 || y >= (int) m_height ){

			return (ubyte) 255;
		}else{
			pixel_type* pixel = m_data + ((y * m_width) + x);

			return pixel->r; // assume all colors are the same intensity
		}
	}

	ubyte Image::at_binary( int x, int y ) const{
		ubyte pixel = this->at_grayscale(x,y);

		return pixel == 0 ? 1 : 0;
	}

	void Image::set( size_type x, size_type y, pixel_type value ){
		pixel_type* pixel_start = m_data + ((y * m_width) + x);
		pixel_start->r = value.r;
		pixel_start->g = value.g;
		pixel_start->b = value.b;
	}

	void Image::set_grayscale( size_t x, size_t y, ubyte value ){
		pixel_type* pixel_start = m_data + ((y * m_width) + x);
		pixel_start->r =
		pixel_start->g =
		pixel_start->b = value;
	}

	void Image::set_binary( size_t x, size_t y, ubyte value ){
		pixel_type* pixel_start = m_data + ((y * m_width) + x);
		pixel_start->r =
		pixel_start->g =
		pixel_start->b = (value == 0 ? 255 : 0);
	}

	void Image::fill( pixel_type uniform ){
		for( uint row = 0; row < height(); ++row ){
			for( uint col = 0; col < width(); ++col ){
				set(col, row, uniform);
			}
		}
	}

	void Image::fill_binary( ubyte uniform ){
		for( uint row = 0; row < height(); ++row ){
			for( uint col = 0; col < width(); ++col ){
				set_binary(col, row, uniform);
			}
		}
	}


}
