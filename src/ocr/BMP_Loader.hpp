/**
 * @file BMP_Loader.hpp
 *
 * @todo Add description
 *
 * @author Matthew Rodusek (matthew.rodusek@gmail.com)
 * @date   September 23, 2015
 *
 */

/*
 * Change Log:
 *
 * Sep 23, 2015: 
 * - BMP_Image_Loader.hpp created
 */
#ifndef OCR_BMP_LOADER_HPP_
#define OCR_BMP_LOADER_HPP_

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif

#include "base_types.hpp"
#include "Image.hpp"

namespace ocr {

  typedef enum image_status_{
    IS_SUCCESS        = 0,
    IS_FILE_NOT_FOUND = 1,
    IS_TYPE_INVALID   = 2
  } image_load_status;

  int load_bmp_image_color( const char* filename, Image** image );

  int load_bmp_image_grayscale( const char* filename, Image** image );

  int load_bmp_image_binary( const char* filename, Image** image );

  bool save_bmp_image( const char* filename, Image* image );

  void destroy_image( Image** image );

}  // namespace ocr



#endif /* OCR_BMP_LOADER_HPP_ */
