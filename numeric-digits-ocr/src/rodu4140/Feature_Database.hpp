/**
 * @file Feature_Database.hpp
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
 * - Feature_Database.hpp created
 */
#ifndef RODU4140_FEATURE_DATABASE_HPP_
#define RODU4140_FEATURE_DATABASE_HPP_

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif

#include <list>
#include "Feature_Vector.hpp"
#include "Feature_Loader.hpp"
#include "Image.hpp"

namespace rodu4140 {

	class Feature_Database{

		//------------------------------------------------------------------------
		// Public Types
		//------------------------------------------------------------------------
	public:

		typedef std::pair<Image, feature_collection> cluster;
		typedef std::vector<cluster>                 cluster_collection;

		//------------------------------------------------------------------------
		// Constructor / Destructor
		//------------------------------------------------------------------------
	public:

		Feature_Database( );

		//------------------------------------------------------------------------
		// Capacity
		//------------------------------------------------------------------------
	public:

		size_t size() const;

		//------------------------------------------------------------------------
		// Access
		//------------------------------------------------------------------------
	public:

		Feature_Database& insert( const Image& data, feature_collection& vector );

		///
		///
		///
		void analyze( Image& image, boundary_collection& bounds, feature_collection& features );

		//-----------------------------------------------------------------------------
		// Private Members
		//-----------------------------------------------------------------------------
	private:

		cluster_collection m_clusters;

	};

	inline size_t Feature_Database::size() const{
		return m_clusters.size();
	}

}  // namespace rodu4140



#endif /* RODU4140_FEATURE_DATABASE_HPP_ */
