/**
 * @file Image Operator.hpp
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
 * - Image Operator.hpp created
 */
#ifndef OCR_KERNEL_IMAGE_OPERATOR_HPP_
#define OCR_KERNEL_IMAGE_OPERATOR_HPP_

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif

#include "Image.hpp"

#include <list>
#include <vector>

namespace ocr {

  /////////////////////////////////////////////////////////////////////////////
  /// @class ocr::Kernel_Image_Operator
  ///
  /// @brief An operator for an image, to be performed on all pixels in an
  ///        image
  ///
  /// Detailed Description
  /////////////////////////////////////////////////////////////////////////////
  class Kernel_Image_Operator  {

    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    typedef Image::pixel_type             pixel_type;
    typedef std::vector<std::vector<f32>> vector_2d_type;

    enum edge_handling{
      edge_extend,  ///< The nearest border pixels are conceptually extended as
                    ///< far as necessary to provide values for the convolution.
                    ///< Corner pixels are extended in 90� wedges.
                    ///< Other edge pixels are extended in lines.

      edge_wrap,    ///< The image is conceptually wrapped (or tiled) and values
                    ///< are taken from the opposite edge or corner

      edge_crop,    ///< Any pixel in the output image which would require values
                    ///< from beyond the edge is skipped

      edge_is_gray  ///< Outside the boundary is treated as gray
    };

    //-------------------------------------------------------------------------
    // Constructor / Destructor / Assignment
    //-------------------------------------------------------------------------
  public:

    ///
    /// @brief Constructs a Kernel Image Operator with the specified name
    ///
    /// @param name   The name of this image process
    /// @param kernel The kernel image
    /// @param e      Method to handle edge-cases of the image
    ///
    Kernel_Image_Operator( const vector_2d_type& kernel,
                           edge_handling e = edge_extend );

    ///
    /// @brief Constructs an operator
    ///
    Kernel_Image_Operator( f32** kernel, std::size_t width, std::size_t height, edge_handling e = edge_extend );

    template<size_t N,size_t M>
    Kernel_Image_Operator( const f32 (&kernel)[N][M], edge_handling e = edge_extend );

    ///
    ///
    ///
    ~Kernel_Image_Operator();

    //-------------------------------------------------------------------------
    // Image Operator API
    //-------------------------------------------------------------------------
  public:

    ///
    /// @brief Chain this operator with the specified operator
    ///
    /// @param op the operator to perform after this operation
    ///
    Kernel_Image_Operator& chain( const Kernel_Image_Operator& op );

    ///
    /// @brief Perform the operation on the supplied image
    ///
    ///
    ///
    Image operate( const Image& image ) const;

    ///
    /// @brief Perform the operation on the supplied image 'n' times
    ///
    ///
    ///
    Image operate( const Image& image, std::size_t n ) const;

    //-------------------------------------------------------------------------
    // Private Methods
    //-------------------------------------------------------------------------
  private:

    ///
    ///
    void do_operation( const Image& in_image, Image& out_image ) const;

    //-------------------------------------------------------------------------
    // Private Member Types
    //-------------------------------------------------------------------------
  private:

    typedef std::list<const Kernel_Image_Operator*> Operators;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    f32**   m_kernel;          ///< The kernel (2D array)
    std::size_t  m_width;           ///< The width of the kernel
    std::size_t  m_height;          ///< The height of the kernel
    f32     m_abs_sum;         ///< The absolute sum of all kernel values
    edge_handling m_edge_case; ///< How to handle pixels on the edge

    Operators m_operators;     ///< List of remaining operators to perform
  };

  //---------------------------------------------------------------------------
  // Inline Definitions
  //---------------------------------------------------------------------------

  template<size_t N, std::size_t M>
  Kernel_Image_Operator::Kernel_Image_Operator( const f32 (&kernel)[N][M], edge_handling e )
    :  m_width(M),
      m_height(N),
      m_abs_sum(0), // set accumulator to 0
      m_edge_case(e)
  {
    // Allocate memory for operator
    m_kernel = new f32*[m_height];
    for( std::size_t i = 0; i < m_height; ++i ){
      m_kernel[i] = new f32[m_width];
    }

    // Copy all elements from the 2D array
    for( std::size_t i = 0; i < m_height; ++i ){
      for( std::size_t j = 0; j < m_width; ++j ){
        m_kernel[i][j] = kernel[i][j];
        m_abs_sum += kernel[i][j];
      }
    }

    // If the sum is less than zero, take the absolute value
    if(m_abs_sum < 0){
      m_abs_sum = -m_abs_sum;
    }
  }

} // namespace ocr

#endif /* OCR_KERNEL_IMAGE_OPERATOR_HPP_ */
