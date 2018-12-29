/**
 * @file Feature_Database.cpp
 *
 * @todo Add description
 *
 * @author Matthew Rodusek (matthew.rodusek@gmail.com)
 * @date   Nov 21, 2015
 *
 */

/*
 * Change Log:
 *
 * Nov 21, 2015: 
 * - Feature_Database.cpp created
 */
#include "Feature_Database.hpp"
#include <iostream>
#include <limits>
#include <map>

namespace ocr{

  Feature_Database::Feature_Database(){}

  Feature_Database& Feature_Database::insert( const Image& glyph, feature_collection& features ){
    cluster result = std::make_pair( glyph, features );

    m_clusters.push_back( result );

    return (*this);
  }

  void Feature_Database::analyze( Image& image,
                                  boundary_collection& bounds,
                                  feature_collection& features )
  {
    feature_collection::iterator  feature_iter = features.begin();
    boundary_collection::iterator bounds_iter  = bounds.begin();

    //------------------------------------------------------------------------
    // Find most likely comparison
    //------------------------------------------------------------------------

    const size_t COMPARISONS = 5;
    typedef std::pair<size_t,double> min_entry;
    typedef std::list<min_entry>     min_list;

    // Iterate through all features
    for( ; feature_iter != features.end(); ++feature_iter ){

      cluster* min_cluster    = &m_clusters.front();

      min_list minimums;

      for( size_t i =0; i < m_clusters.size() ; ++i ){

        cluster& clust = m_clusters[i];

        for( Feature_Vector& vec : clust.second ){

          Feature_Vector difference = (vec - (*feature_iter));
          double diff = difference.magnitude();

          // Record the minimal location if the list is empty, or if the difference is less
          if( minimums.empty() ){

            // record index of occurrence
            min_entry minimum = std::make_pair(i,diff);
            minimums.push_back(minimum);

          }else if ( diff < minimums.front().second ){

            // record index of occurrence
            min_entry minimum = std::make_pair(i,diff);
            minimums.push_back(minimum);

            // Keep at most COMPARISONS entries
            if(minimums.size() > COMPARISONS){
              minimums.pop_front();
            }

          }
        }

      }

      std::vector<size_t> most_common;
      // Initialize vector as 0
      for( size_t i = 0; i < m_clusters.size(); ++i ){
        most_common.push_back(0);
      }
      // count most common
      for( min_list::iterator iter = minimums.begin(); iter != minimums.end(); ++iter ){
        most_common[iter->first]++;
      }

      // Find the index of the most likely cluster
      size_t minimal_entry_index      = 0;
      size_t minimal_entry_occurrence = 0;

      for( size_t i = 0; i < most_common.size(); ++i ){
        if( most_common[i] > minimal_entry_occurrence ){
          minimal_entry_index = i;
          minimal_entry_occurrence = most_common[i];
        }
      }

      min_cluster = &m_clusters[minimal_entry_index];

      //------------------------------------------------------------------------
      // Stretch the output glyph
      //------------------------------------------------------------------------

      // Data for stretching the output glyph
      Image& glyph             = min_cluster->first;
      const size_t from_height = glyph.height();
      const size_t from_width  = glyph.width();
      const size_t to_height   = bounds_iter->bottom - bounds_iter->top + 1;
      const size_t to_width    = bounds_iter->right  - bounds_iter->left + 1;

      double x_ratio = from_width  / (double) to_width;
      double y_ratio = from_height / (double) to_height;

      for( size_t y = 0; y < to_height; ++y ){
        for( size_t x = 0; x < to_width; ++x ){
          // Calculate the stretched x/y coordinates
          int x_in = (int) std::floor( x_ratio * x );
          int y_in = (int) std::floor( y_ratio * y );

          // Calculate the outputted x/y coordinates
          int x_out = x + bounds_iter->left - 1;
          int y_out = y + bounds_iter->top  - 1;

          if( glyph.at_binary(x_in, y_in) ){
            image.set_binary( x_out, y_out, 1 );
          }
        }
      }
      bounds_iter++;
    }
  }

}


