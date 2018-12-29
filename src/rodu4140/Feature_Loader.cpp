/**
 * @file Feature_Loader.cpp
 *
 * @todo Add description
 *
 * @author Matthew Rodusek (matthew.rodusek@gmail.com)
 * @date   Nov 17, 2015
 *
 */

/*
 * Change Log:
 *
 * Nov 17, 2015: 
 * - Feature_Loader.cpp created
 */

#include "Feature_Loader.hpp"
#include <iostream>

namespace rodu4140 {

  //--------------------------------------------------------------------------
  // Stream Operators
  //--------------------------------------------------------------------------

  std::ostream& operator << (std::ostream& o, const boundary& b ){
    o << "((" << b.left << ", " << b.top << "),(" << b.right << "," << b.bottom << "))";
    return o;
  }

  //--------------------------------------------------------------------------
  // 2D Allocations
  //--------------------------------------------------------------------------

  template<typename T>
  inline T** new_2d( size_t rows, size_t cols ){
    T** result = new T*[rows];
    for( size_t i = 0; i < rows; ++i ){
      result[i] = new T[cols];
    }
    return result;
  }

  //--------------------------------------------------------------------------

  template<typename T>
  inline void delete_2d( T** ptr, size_t rows ){
    for( size_t i = 0; i < rows; ++i ){
      delete[] ptr[i];
    }
    delete [] ptr;
  }

  //--------------------------------------------------------------------------
  // Recursive Expansion
  //--------------------------------------------------------------------------

  void expand_map_aux( int label, int** label_map, const Image& image, int row, int col, boundary& bounds ){
    // Stop condition 1: out of bounds
    if( row < 0 || col < 0){
      return;
    }
    if( ((size_t) col) >= image.width() || ((size_t) row) >= image.height()){
      return;
    }

    // Stop condition 2: Pixel is 0
    if(!image.at_binary(col,row)) // at (x,y)
      return;

    // Stop condition 3: label map already labeled
    if(label_map[row][col])
      return;

    // Adjust the known boundaries.
    if(col < bounds.left){
      bounds.left  = col;
    }
    if(col > bounds.right){
      bounds.right = col;
    }
    if(row < bounds.top){
      bounds.top    = row;
    }
    if(row > bounds.bottom){
      bounds.bottom = row;
    }

    // Assign the label
    label_map[row][col] = label;

    // Recursively expand top pixels
    expand_map_aux( label, label_map, image, row - 1, col - 1, bounds);
    expand_map_aux( label, label_map, image, row,     col - 1, bounds);
    expand_map_aux( label, label_map, image, row + 1, col - 1, bounds);

    // Recursively expand outside pixels
    expand_map_aux( label, label_map, image, row - 1, col,     bounds);
    expand_map_aux( label, label_map, image, row + 1, col,     bounds);

    // Recursively expand bottom 3 pixels
    expand_map_aux( label, label_map, image, row - 1, col + 1, bounds);
    expand_map_aux( label, label_map, image, row,     col + 1, bounds);
    expand_map_aux( label, label_map, image, row + 1, col + 1, bounds);
  }

  //--------------------------------------------------------------------------

  boundary expand_map( int label, int** label_map, const Image& image, int row, int col ){
    boundary bound;
    // Assign the boundary to just the first pixel coordinate
    bound.left   = col;
    bound.right  = col;
    bound.top    = row;
    bound.bottom = row;

    expand_map_aux( label, label_map, image, row, col, bound );
    return bound;
  }

  //--------------------------------------------------------------------------

  size_t count_labeled( int label, int** label_map, int start_row, int start_col, int end_row, int end_col ){
    size_t count = 0;
    for( int i = start_row; i <= end_row; ++i ){
      for( int j = start_col; j <= end_col; ++j ){
        if( label_map[i][j] == label ){
          ++count;
        }
      }
    }
    return count;
  }

  //--------------------------------------------------------------------------

  void load_features( const Image& image,
                      feature_collection&  features,
                      boundary_collection& bounds,
                      size_t horizontal_divs,
                      size_t vertical_divs ){

    int** label_map = new_2d<int>( image.height(), image.width() );

    // Set the 2D array to 0
    for( size_t row = 0; row < image.height(); ++row ){
      for( size_t col = 0; col < image.width(); ++col ){
        label_map[row][col] = 0;
      }
    }

    int label = 1;

    // Iterate through all pixels, expanding any discovered regions
    for( size_t row = 0; row < image.height(); ++row ){
      for( size_t col = 0; col < image.width(); ++col ){

        ubyte pix = image.at_binary(col,row); // x y

        // Only expand as a map if not already labeled
        if( pix && !label_map[row][col]){
          boundary b = expand_map( label++, label_map, image, (int) row, (int) col );
          bounds.push_back(b);
        }
      }
    }

    //------------------------------------------------------------------------

    // Analyze the discovered vectors
    boundary_collection::iterator iter = bounds.begin();
    for( int current_label = 1; iter != bounds.end(); ++iter, ++current_label ){

      Feature_Vector::feature_collection feat;

      const size_t width  = iter->right  - iter->left + 1;
      const size_t height = iter->bottom - iter->top  + 1;

      size_t black  = 0;
      size_t total  = 0;

      black = count_labeled( current_label, label_map, iter->top, iter->left, iter->bottom, iter->right );
      total = width*height;

      // Push back the total in that region
      feat.push_back( black / (double)(total) );


      int new_height = height + (vertical_divs   - (height % vertical_divs  ));
      int new_width  = width  + (horizontal_divs - (width  % horizontal_divs));

      double y_ratio = height / (double) new_height;
      double x_ratio = width  / (double) new_width;

      int y_step = new_height / vertical_divs;
      int x_step = new_width  / horizontal_divs;

      for( size_t y = 0; y < vertical_divs; ++y ){
        for( size_t x = 0; x < horizontal_divs; ++x ){
          int row_start = std::max(iter->top  + (int)(std::floor(y     * y_step * y_ratio))    , iter->top );
          int row_end   = std::max(iter->top  + (int)(std::floor((y+1) * y_step * y_ratio)) - 1, row_start );
          int col_start = std::max(iter->left + (int)(std::floor(x     * x_step * x_ratio))    , iter->left);
          int col_end   = std::max(iter->left + (int)(std::floor((x+1) * x_step * x_ratio)) - 1, col_start );

          black = count_labeled( current_label, label_map, row_start, col_start, row_end, col_end );
          total = (col_end - col_start + 1) * (row_end - row_start + 1);

          feat.push_back( black / (double)(total) );

        }
      }


      //----------------------------------------------------------------------
      // Horizontal Histogram
      //----------------------------------------------------------------------

      // Scale the image up to an increment of DIVS to calculate the amount

      for( size_t i = 0; i < vertical_divs; ++i ){
        int row_start = std::max(iter->top + (int)(std::floor(i     * y_step * y_ratio))    , iter->top );
        int row_end   = std::max(iter->top + (int)(std::floor((i+1) * y_step * y_ratio)) - 1, row_start );


        black = count_labeled( current_label, label_map, row_start, iter->left, row_end, iter->right );
        total = width * (row_end - row_start + 1);

        feat.push_back( black / (double)(total) );
      }

      //----------------------------------------------------------------------
      // Vertical Histogram
      //----------------------------------------------------------------------

      for( size_t i = 0; i < horizontal_divs; ++i ){
        int col_start = std::max(iter->left + (int)(std::floor(i     * x_step * x_ratio))    , iter->left);
        int col_end   = std::max(iter->left + (int)(std::floor((i+1) * x_step * x_ratio)) - 1, col_start );

        black = count_labeled( current_label, label_map, iter->top, col_start, iter->bottom, col_end );
        total = height * (col_end - col_start + 1);

        feat.push_back( black / (double)(total) );
      }

      features.push_back( Feature_Vector( feat ) );
    }

    delete_2d(label_map, image.height() );
  }


}  // namespace rodu4140

