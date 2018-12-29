/**
 * @file Image_Operator.cpp
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
 * - Image_Operator.cpp created
 */
#include "Kernel_Image_Operator.hpp"

namespace ocr {

  //---------------------------------------------------------------------------
  // Constructor / Destructor
  //---------------------------------------------------------------------------

  Kernel_Image_Operator::Kernel_Image_Operator( const vector_2d_type& kernel,
                                                edge_handling e )
    :  m_width(kernel[0].size()),
      m_height(kernel.size()),
      m_abs_sum(0),
      m_edge_case(e)
  {
    m_kernel = new f32*[m_height];
    for( size_t i = 0; i < m_height; ++i ){
      m_kernel[i] = new f32[m_width];
    }

    for( size_t i = 0; i < m_width; ++i ){
      for( size_t j = 0; j < m_height; ++j ){
        m_kernel[i][j] = kernel[i][j];
        m_abs_sum += m_kernel[i][j];
      }
    }

    // If the sum is less than zero, take the absolute value
    if(m_abs_sum < 0){
      m_abs_sum = -m_abs_sum;
    }
  }

  Kernel_Image_Operator::Kernel_Image_Operator( f32** kernel, size_t width, size_t height, edge_handling e)
    :  m_width(width),
      m_height(height),
      m_abs_sum(0), // set accumulator to 0
      m_edge_case(e)
  {
    // Allocate memory for operator
    m_kernel = new f32*[m_height];
    for( size_t i = 0; i < m_height; ++i ){
      m_kernel[i] = new f32[m_width];
    }

    // Copy all elements from the 2D array
    for( size_t i = 0; i < m_height; ++i ){
      for( size_t j = 0; j < m_width; ++j ){
        m_kernel[i][j] = kernel[i][j];
        m_abs_sum += m_kernel[i][j];
      }
    }

    // If the sum is less than zero, take the absolute value
    if(m_abs_sum < 0){
      m_abs_sum = -m_abs_sum;
    }
  }

  Kernel_Image_Operator::~Kernel_Image_Operator(){

    // Delete Memory for operator
    for( size_t i = 0; i < m_height; ++i ){
      delete [] m_kernel[i];
    }
    delete [] m_kernel;
  }

  //---------------------------------------------------------------------------
  // Operator
  //---------------------------------------------------------------------------

  Image Kernel_Image_Operator::operate( const Image& image ) const{

    // Double buffered image operations
    Image buffers[] = {
      Image(image),
      Image(image.width(),image.height())
    };
    Image* active_buffer   = &buffers[0];
    Image* inactive_buffer = &buffers[1];

    //-------------------------------------------------------------------------

    // Perform the operation, then swap the buffers
    this->do_operation( *active_buffer, *inactive_buffer );
    std::swap( active_buffer, inactive_buffer );

    //-------------------------------------------------------------------------

    // Perform the chained operation, then swap the buffers
    Operators::const_iterator iter = m_operators.cbegin();
    for( ; iter != m_operators.cend(); ++iter ){
      (*iter)->do_operation( *active_buffer, *inactive_buffer );
      std::swap( active_buffer, inactive_buffer );
    }

    //-------------------------------------------------------------------------

    return *active_buffer;
  }

  Image Kernel_Image_Operator::operate( const Image& image, size_t n ) const{

    // Double buffered
    Image buffers[] = {
      Image(image),
      Image(image.width(),image.height())
    };
    Image* active_buffer   = &buffers[0];
    Image* inactive_buffer = &buffers[1];

    //-------------------------------------------------------------------------

    // Do operation n times first
    for( size_t i = 0; i < n; ++i ){
      this->do_operation( *active_buffer, *inactive_buffer );
      // swap the buffer pointers
      std::swap( active_buffer, inactive_buffer );
    }

    //-------------------------------------------------------------------------

    Operators::const_iterator iter = m_operators.cbegin();
    for( ; iter != m_operators.cend(); ++iter ){
      (*iter)->do_operation( *active_buffer, *inactive_buffer );
      // swap the buffer
      std::swap( active_buffer, inactive_buffer );
    }

    //-------------------------------------------------------------------------

    return *active_buffer;
  }

  //---------------------------------------------------------------------------

  //
  // This algorithm runs in O(m*n*i*j) where m and n are the width and height
  // of the image respectively, and i and j are the width and height of the
  // kernel matrix.
  //
  // Sadly there is no other way around dealing with this, due to the i*j being
  // necessary for the accumulator.
  //
  void Kernel_Image_Operator::do_operation( const Image& in_image, Image& out_image ) const{

    const u32 offset_i = (m_width / 2);
    const u32 offset_j = (m_height / 2);

    //-------------------------------------------------------------------------

    // Allocate 2D array
    pixel_type** values = new pixel_type*[m_height];
    for( size_t i = 0; i < m_height; ++i ){
      values[i] = new pixel_type[m_width];
    }

    //-------------------------------------------------------------------------

    for( u32 x = 0; x < in_image.width(); ++x ){
      for( u32 y = 0; y < in_image.height(); ++y ){
        const bool on_top_boundary    = (y < offset_j);
        const bool on_right_boundary  = (x > (in_image.width() - 1 - offset_i));
        const bool on_bottom_boundary = (y > (in_image.height() - 1 - offset_j));
        const bool on_left_boundary   = (x < offset_i);

        s32 accumulator_r = 0;
        s32 accumulator_g = 0;
        s32 accumulator_b = 0;

        //---------------------------------------------------------------------

        // If edge case crops, then just take the original pixel
        if( (m_edge_case == edge_crop) && (
              (x < offset_i) ||
              (x > (in_image.width()  - offset_i - 1)) ||
              (y < offset_j) ||
              (y > (in_image.height() - offset_j - 1))
            )
          ){
          out_image.set( x, y, pixel_type::BLACK );
        }else{

          // Loop through the operators
          for( s32 i = -offset_i; i <= (s32) offset_i; ++i ){
            for( s32 j = -offset_j; j <= (s32) offset_j; ++j ){
              const s32 index_i = i + offset_i;
              const s32 index_j = j + offset_j;

              // If on edge case, handle edge case
              if( (on_top_boundary  && (j < 0)) ||
                  (on_left_boundary && (i < 0)) ||
                  (on_right_boundary && (i > 0)) ||
                  (on_bottom_boundary && (j > 0))
                ){

                switch(m_edge_case){

                case edge_extend:
                  values[index_i][index_j] = in_image.at(
                    on_left_boundary ? 0 : on_right_boundary  ? in_image.width() - 1  : x,
                    on_top_boundary  ? 0 : on_bottom_boundary ? in_image.height() - 1 : y
                  );
                  break;

                //---------------------------------------------------------------

                case edge_is_gray:
                  values[index_i][index_j] = pixel_type::GRAY;
                  break;

                //---------------------------------------------------------------

                case edge_wrap:
                  values[index_i][index_j] = in_image.at(
                    on_left_boundary ? in_image.width()  + i - 1 : on_right_boundary  ? i : x,
                    on_top_boundary  ? in_image.height() + j - 1 : on_bottom_boundary ? j : y
                  );
                  break;

                //---------------------------------------------------------------

                default:
                  // Shouldn't happen
                  break;
                }
              }else{
                values[index_i][index_j] = in_image.at(x+i,y+j);
              }
            }
          }

          //---------------------------------------------------------------------

          // Calculate the sum of all operations
          for( size_t i = 0; i < m_width; ++i ){
            for( size_t j = 0; j < m_height; ++j ){
              const size_t ki = m_width - i - 1;
              const size_t kj = m_height - j - 1;
              accumulator_r += m_kernel[ki][kj] * values[i][j].r;
              accumulator_g += m_kernel[ki][kj] * values[i][j].g;
              accumulator_b += m_kernel[ki][kj] * values[i][j].b;
            }
          }

          // Divide all sums by the absolute sum, if the sum is not 0
          if(m_abs_sum){
            accumulator_r /= m_abs_sum;
            accumulator_g /= m_abs_sum;
            accumulator_b /= m_abs_sum;
          }

          //---------------------------------------------------------------------

          // Clamp the sum between (0,255)
          pixel_type sum(
            ((accumulator_r > 255) ? 255 : ((accumulator_r < 0) ? 0 : accumulator_r)),
            ((accumulator_g > 255) ? 255 : ((accumulator_g < 0) ? 0 : accumulator_g)),
            ((accumulator_b > 255) ? 255 : ((accumulator_b < 0) ? 0 : accumulator_b))
          );

          //---------------------------------------------------------------------

          out_image.set( x, y, sum );
        }
      }
    }

    //-------------------------------------------------------------------------

    // Delete 2D array
    for( size_t i = 0; i < m_height; ++i ){
      delete [] values[i];
    }
    delete [] values;

  }

  //---------------------------------------------------------------------------

  Kernel_Image_Operator& Kernel_Image_Operator::chain( const Kernel_Image_Operator& op ){
    m_operators.push_back(&op);
    return (*this);
  }

}  // namespace ocr
