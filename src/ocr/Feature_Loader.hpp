/**
 * @file Feature_Loader.hpp
 *
 * @todo Add description
 *
 * @author Matthew Rodusek (matthew.rodusek@gmail.com)
 * @date   Nov 16, 2015
 *
 */

/*
 * Change Log:
 *
 * Nov 16, 2015: 
 * - Feature_Loader.hpp created
 */
#ifndef OCR_FEATURE_LOADER_HPP_
#define OCR_FEATURE_LOADER_HPP_

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif

#include "Image.hpp"
#include "Feature_Vector.hpp"

#include <vector>
#include <cstddef> // std::size_t
#include <iosfwd>  // std::ostream fwd

namespace ocr {

  struct boundary{
    int top, left, bottom, right;
  };

  typedef std::vector<Feature_Vector> feature_collection;
  typedef std::vector<boundary>       boundary_collection;

  std::ostream& operator << (std::ostream& o, const boundary& b );

  ///
  ///
  ///
  ///
  void load_features( const Image& image,
                      feature_collection&  features,
                      boundary_collection& bounds,
                      std::size_t horizontal_divs = 10,
                      std::size_t vertical_divs   = 10 );

}  // namespace ocr



#endif /* OCR_FEATURE_LOADER_HPP_ */
