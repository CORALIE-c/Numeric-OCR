/**
 * @file BMP_Loader.cpp
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
 * - BMP_Loader.cpp created
 */
#include "BMP_Loader.hpp"
#include "base_types.hpp"
#include "Image.hpp"

#include <cmath>
#include <fstream>
#include <ostream>

namespace ocr {

  //---------------------------------------------------------------------------
  // Structures
  //---------------------------------------------------------------------------

  typedef struct bmp_file_header_{
    u8  type[2];   ///< 2 bytes: "BM" in little-endian order
    u32 size;      ///< 4 bytes: Size of the bmp file in bytes
    u16 reserved1; ///< 2 bytes: reserved for the program that makes the bitmap
    u16 reserved2; ///< 2 bytes: reserved for the program that makes the bitmap
    u16 offset;    ///< 2 bytes: offset from 0 to pixel map
  } bmp_file_header;

  //---------------------------------------------------------------------------

  typedef struct bmp_info_header_{
    u32 size;         ///< 4 bytes: Size of this header (40 bytes)
    s32 width;        ///< 4 bytes: Bitmap width in pixels
    s32 height;       ///< 4 bytes: Bitmap height in pixels
    u16 planes;       ///< 2 bytes: the number of color planes (must be 1)
    u16 bpp;          ///< 2 bytes: the number of bits per pixel, which is the color depth of the image.

    u32 compression;  ///< 4 bytes: compression method being used
    u32 image_size;   ///< 4 bytes: the image size. This is the size of the raw bitmap data
    s32 x_resolution; ///< 4 bytes: the horizontal resolution of the image
    s32 y_resolution; ///< 4 bytes: the vertical resolution of the image

    u32 colors;           ///< 4 bytes: the number of colors in the color palette, or 0 to default to 2^n
    u32 important_colors; ///< 4 bytes: the number of important colors used, or 0 when every color is important; generally ignored
  } bmp_info_header;

  //---------------------------------------------------------------------------

  typedef enum bmp_compression_methods_{
    BI_RGB,
    BI_RLE8,
    BI_RLE4,
    BI_BITFIELDS,
    BI_JPEG,
    BI_PNG,
    BI_ALPHABITFIELDS,
    BI_CMYK,
    BI_CMYKRLE8,
    BI_CMYKRLE4
  } bmp_compression_methods;

  //---------------------------------------------------------------------------

  static const std::size_t bmp_file_header_size = 14;
  static const std::size_t bmp_info_header_size = 40;
  static const std::size_t bmp_bit_mask_size    = 12;

  //---------------------------------------------------------------------------

  static void read_bmp_headers( ubyte* buffer,
                                bmp_file_header* file_header,
                                bmp_info_header_* info_header ){

    // Read file header
    file_header->type[0]   = (u16) buffer[0]; // ( (buffer[0] << 8) | buffer[1] );
    file_header->type[1]   = (u16) buffer[1];
    //file_header.type   = (*(u16*)(buffer + 0x00));
    file_header->size      = (*(u32*)(buffer + 0x02));
    file_header->reserved1 = (*(u16*)(buffer + 0x06));
    file_header->reserved2 = (*(u16*)(buffer + 0x08));
    file_header->offset    = (*(u32*)(buffer + 0x0A));

    // Read info_header
    info_header->size      = (*(u32*)(buffer + 0x0E));
    info_header->width     = (*(s32*)(buffer + 0x12));
    info_header->height    = (*(s32*)(buffer + 0x16));
    info_header->planes    = (*(u16*)(buffer + 0x1A));
    info_header->bpp       = (*(u16*)(buffer + 0x1C));

    info_header->compression  = (*(u32*)(buffer + 0x1E));
    info_header->image_size   = (*(u32*)(buffer + 0x22));
    info_header->x_resolution = (*(u32*)(buffer + 0x26));
    info_header->y_resolution = (*(u32*)(buffer + 0x2A));

    info_header->colors           = (*(u32*)(buffer + 0x2E));
    info_header->important_colors = (*(u32*)(buffer + 0x32));

  }

  //---------------------------------------------------------------------------
  // Load images
  //---------------------------------------------------------------------------

  int load_bmp_image_color( const char* filename, Image** image ){
    typedef Image::pixel_type pixel_type;

    std::fstream file;
    file.open(filename, std::ios::in | std::ios::binary );

    // If file doesn't open, throw exception
    if(!file.is_open()){
      return IS_FILE_NOT_FOUND;
    }

    bmp_file_header file_header;
    bmp_info_header info_header;
    ubyte           buffer[54];

    file.read( (char*) buffer, sizeof(buffer) );

    //-------------------------------------------------------------------------
    // Read headers
    //-------------------------------------------------------------------------

    read_bmp_headers( buffer, &file_header, &info_header );

    //-------------------------------------------------------------------------

    // Ensure file header is accurate
    if((file_header.type[0] != 0x42) || (file_header.type[1] != 0x4D) ){
      return IS_TYPE_INVALID;
    }

    // If compression is BI_BITFIELDS, seek ahead past the bit mask
    if(info_header.compression == BI_BITFIELDS ){
      file.seekg( bmp_file_header_size + bmp_info_header_size + bmp_bit_mask_size );
    }

    pixel_type* data = new pixel_type[ info_header.width * info_header.height ];

    // 24 & 32 bit bitmaps store data immediately after headers; no table
    if(( info_header.bpp == 24) || (info_header.bpp == 32)){
      // Size of each row (account for padding)
      u32 row_size = ((info_header.width * 3 + 3) & ~3);

      // Allocate storage for the row
      ubyte* row_buffer = new ubyte[row_size];

      // Read each pixel from pixel buffer; store in data
      for( s32 row = 0; row < info_header.height; ++row ){
        file.read( (char*) row_buffer, row_size );

        for( s32 col = 0; col < info_header.width; ++col ){

          // Bitmaps store data from bottom up
          const u32 index = (((info_header.height - row - 1) * info_header.width) + col);

          // Bitmap stores in BGR format
          data[index].r = row_buffer[3*col+2];
          data[index].g = row_buffer[3*col+1];
          data[index].b = row_buffer[3*col];
        }
      }

      delete [] row_buffer;
    }else{
      file.close();
      return IS_TYPE_INVALID;
    }
    file.close();

    *image = new Image( info_header.width, info_header.height, data );

    return IS_SUCCESS;
  }

  //---------------------------------------------------------------------------

  int load_bmp_image_grayscale( const char* filename, Image** image ){
    typedef Image::pixel_type pixel_type;

    int status;
    if( (status = load_bmp_image_color( filename, image )) != IS_SUCCESS ){
      return status;
    }
    for( std::size_t i = 0; i < (*image)->width(); ++i){
      for( std::size_t j = 0; j < (*image)->height(); ++j ){
        pixel_type old_pixel = (*image)->at(i,j);
        pixel_type new_pixel;

        u32 average = (old_pixel.r + old_pixel.g + old_pixel.b) / 3;

        new_pixel.r = (ubyte) average;
        new_pixel.g = (ubyte) average;
        new_pixel.b = (ubyte) average;

        (*image)->set( i, j, new_pixel );
      }
    }

    return status;
  }

  //---------------------------------------------------------------------------

  int load_bmp_image_binary( const char* filename, Image** image ){
    typedef Image::pixel_type pixel_type;

    int status;
    if( (status = load_bmp_image_color( filename, image )) != IS_SUCCESS ){
      return status;
    }
    for( std::size_t i = 0; i < (*image)->width(); ++i){
      for( std::size_t j = 0; j < (*image)->height(); ++j ){
        pixel_type old_pixel = (*image)->at(i,j);
        pixel_type new_pixel;

        // To load an image in binary,
        ubyte value = (((old_pixel.r + old_pixel.g + old_pixel.b) == 0) ? 0 : 255);

        new_pixel.r = value;
        new_pixel.g = value;
        new_pixel.b = value;

        (*image)->set( i, j, new_pixel );
      }
    }

    return status;
  }

  //---------------------------------------------------------------------------
  // Save Image
  //---------------------------------------------------------------------------

  bool save_bmp_image( const char* filename, Image* image ){
    typedef Image::pixel_type pixel_type;

    std::fstream file;
    file.open(filename, std::ios::out | std::ios::binary );

    // If file doesn't open, throw exception
    if(!file.is_open()){
      return false;
    }

    //-------------------------------------------------------------------------

    u32 row_size   = ((image->width() * 3 + 3) & ~3);
    u32 image_size = (image->height() * row_size);
    ubyte buffer[54];

    // Write file header
    (*(u8* )(buffer + 0x00)) = 'B';
    (*(u8* )(buffer + 0x01)) = 'M';
    (*(u32*)(buffer + 0x02)) = image_size + bmp_file_header_size + bmp_info_header_size;
    (*(u16*)(buffer + 0x06)) = 0;
    (*(u16*)(buffer + 0x08)) = 0;
    (*(u32*)(buffer + 0x0A)) = bmp_file_header_size + bmp_info_header_size;

    // Write info header
    (*(u32*)(buffer + 0x0E)) = bmp_info_header_size;  // header size
    (*(s32*)(buffer + 0x12)) = image->width();        // image width
    (*(s32*)(buffer + 0x16)) = image->height();       // image height
    (*(u16*)(buffer + 0x1A)) = 1;                     // planes
    (*(u16*)(buffer + 0x1C)) = 24;                    // bpp

    (*(u32*)(buffer + 0x1E)) = BI_RGB;     // Compression
    (*(u32*)(buffer + 0x22)) = image_size; // Image size
    (*(u32*)(buffer + 0x26)) = 0;          // x resolution
    (*(u32*)(buffer + 0x2A)) = 0;          // y resolution

    (*(u32*)(buffer + 0x2E)) = 0;          // colors
    (*(u32*)(buffer + 0x32)) = 0;          // important colors

    // write the headers to the file
    file.write( (char*) buffer, sizeof(buffer) );

    //-------------------------------------------------------------------------

    // Allocate data array for saving new image
    ubyte* row_buffer = new ubyte[row_size];

    pixel_type temp;

    for( u32 row = 0; row < image->height(); ++row ){
      for( u32 col = 0; col < image->width(); ++col ){

        // BMP saves rows of from bottom-up
        temp = image->at( col, image->height() - row - 1 );
        row_buffer[col*3+2] = temp.r;
        row_buffer[col*3+1] = temp.g;
        row_buffer[col*3]   = temp.b;
      }
      file.write( (char*) row_buffer, row_size );
    }

    delete [] row_buffer;

    file.close();
    return true;
  }

  //---------------------------------------------------------------------------
  // Destroy Image
  //---------------------------------------------------------------------------

  void destroy_image( Image** image ){
    delete (*image);
    (*image) = NULL;
  }

  //---------------------------------------------------------------------------

}  // namespace ocr

