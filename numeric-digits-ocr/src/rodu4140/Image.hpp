/**
 * @file Image.hpp
 *
 * @todo Add description
 *
 * @author Matthew Rodusek (matthew.rodusek@gmail.com)
 * @date   Sep 21, 2015
 *
 */

/*
 * Change Log:
 *
 * Sep 21, 2015: 
 * - Image.hpp created
 */
#ifndef RODU4140_IMAGE_HPP_
#define RODU4140_IMAGE_HPP_

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif

#include "base_types.hpp"
#include "Color.hpp"

namespace rodu4140 {

  /////////////////////////////////////////////////////////////////////////////
  /// @class rodu4140::Image
  ///
  /// @brief This object is an abstract representation of an image
  ///
  /////////////////////////////////////////////////////////////////////////////
  class Image {

    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    typedef size_t    size_type;
    typedef Color_RGB pixel_type;

    //-------------------------------------------------------------------------
    // Private Constructor
    //-------------------------------------------------------------------------
  public:

    ///
    /// @brief Constructs an image of the specified width and height with the
    ///        specified data.
    ///
    /// This constructor is private so that it can only be accessed by the
    /// friend class, Image_Loader.
    ///
    /// @param width  The width of the image
    /// @param height The height of the image
    /// @param data   Pointer to the data to use for the image
    ///
    Image( size_type width, size_type height, pixel_type* data );

    //-------------------------------------------------------------------------
    // Constructor / Destructor / Assignment
    //-------------------------------------------------------------------------
  public:

    ///
    /// @brief Constructs a blank image of the specified width and height.
    ///
    /// @param width  The width of the image
    /// @param height The height of the image
    /// @param data   Pointer to the data to use for the image
    ///
    Image( size_type width, size_type height );

    ///
    /// @brief Constructs an image by copying the data from @p x to @p this
    ///
    /// @param x the image to copy
    ///
    Image( const Image& x );

#ifdef RODU4140_COMPILER_HAS_CPP11_RVALUE

    ///
    /// @brief Cosntructs an image by moving the data from @p x to @p this
    ///
    /// @param x the image to move
    ///
    Image( Image&& x );

#endif

    ///
    /// @brief Destructor for the image object
    ///
    virtual ~Image();

    //-------------------------------------------------------------------------
    // Capacity
    //-------------------------------------------------------------------------
  public:

    ///
    /// @brief Return the size of this image in bytes
    ///
    /// @return the size of the image in bytes
    ///
    virtual size_type size() const ;

    ///
    /// @brief Return the width of this image in pixels
    ///
    /// @return the width of the image
    ///
    virtual size_type width() const ;

    ///
    /// @brief Return the height of this image in pixels
    ///
    /// @return the height of the image
    ///
    virtual size_type height() const ;

    //------------------------------------------------------------------------
    // Element Access
    //------------------------------------------------------------------------
  public:

    ///
    /// @brief Returns a pointer to the internal data
    ///
    /// @return pointer to the internal data
    ///
    pixel_type* ptr() ;

    ///
    /// @brief Returns a constant pointer to the internal data
    ///
    /// @return const pointer to the internal data
    ///
    const pixel_type* ptr() const ;

    ///
    /// @brief Retrieve the pixel at the specified coordinates
    ///
    /// @param x the x-coordinate to grab the pixel from
    /// @param y the y-coordinate to grab the pixel from
    ///
    /// @return the color value of the specified pixel
    ///
    virtual pixel_type at( int x, int y ) const;

    ubyte at_grayscale( int x, int y ) const;

    ubyte at_binary( int x, int y ) const;

    ///
    /// @brief Sets the pixel at the specified coordinates
    ///
    /// @param x the x-coordinate to set the pixel
    /// @param y the y-coordinate to set the pixel
    ///
    virtual void set( size_type x, size_type y, pixel_type value );

    void set_grayscale( size_t x, size_t y, ubyte value );

    void set_binary( size_t x, size_t y, ubyte value );

    ///
    /// @brief FIlls this image with the uniform pixel value
    ///
    /// @param uniform the pixel to fill this image with.
    ///
    virtual void fill( pixel_type uniform );

    virtual void fill_binary( ubyte uniform );

    //-------------------------------------------------------------------------
    // Protected Members
    //-------------------------------------------------------------------------
  protected:

    size_type   m_width;  ///< Width of the image
    size_type   m_height; ///< Height of the image
    pixel_type* m_data;   ///< Bytestream of data
  };

  //---------------------------------------------------------------------------
  // Inline Definitions
  //---------------------------------------------------------------------------

  inline Image::pixel_type* Image::ptr() {
    return m_data;
  }

  inline const Image::pixel_type* Image::ptr() const {
    return m_data;
  }

  inline Image::size_type Image::size() const{
    return (m_width * m_height * sizeof(pixel_type));
  }

  inline Image::size_type Image::width() const{
    return m_width;
  }

  inline Image::size_type Image::height() const{
    return m_height;
  }

}  // namespace rodu4140

#endif /* RODU4140_IMAGE_HPP_ */
