/*
 * Copyright (C) 2008 Fabien Chereau
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef _STELSKYPOLYGON_HPP_
#define _STELSKYPOLYGON_HPP_

#include "StelSphereGeometry.hpp"
#include "MultiLevelJsonBase.hpp"

#include <QTimeLine>

class StelCore;

//! Contain all the credits for a given server hosting the data
class ServerCredits
{
public:
	//! Very short credit to display in the loading bar
	QString shortCredits;
	
	//! Full credits
	QString fullCredits;
	
	//! The URL where to get more info about the server
	QString infoURL;
};

//! Contains all the credits for the creator of the polygon collection
class DataSetCredits
{
public:
	//! Very short credit to display in the loading bar
	QString shortCredits;
	
	//! Full credits
	QString fullCredits;
	
	//! The URL where to get more info about the data collection
	QString infoURL;
};

//! Base class for any polygon with a fixed position in the sky
class StelSkyPolygon : public MultiLevelJsonBase
{
	Q_OBJECT
	
public:
	//! Default constructor
	StelSkyPolygon() {initCtor();}
	
	//! Constructor
	StelSkyPolygon(const QString& url, StelSkyPolygon* parent=NULL);
	//! Constructor
	StelSkyPolygon(const QVariantMap& map, StelSkyPolygon* parent);
	
	//! Destructor
	~StelSkyPolygon();

	//! Draw the image on the screen.
	void draw(StelCore* core);
	
	//! Return the dataset credits to use in the progress bar
	DataSetCredits getDataSetCredits() const {return dataSetCredits;}
	
	//! Return the server credits to use in the progress bar
	ServerCredits getServerCredits() const {return serverCredits;}

	//! Convert the polygon informations to a map following the JSON structure.
	//! It can be saved as JSON using the StelJsonParser methods.
	QVariantMap toQVariantMap() const;
	
protected:
	//! Minimum resolution at which the next level needs to be loaded in degree/pixel
	float minResolution;
	
	//! The credits of the server where this data come from
	ServerCredits serverCredits;
	
	//! The credits for the data set
	DataSetCredits dataSetCredits;
	
	//! Direction of the vertices of the convex hull in ICRS frame
	QList<SphericalConvexPolygon> skyConvexPolygons;
	
protected:

	//! Load the polygon from a valid QVariantMap
	virtual void loadFromQVariantMap(const QVariantMap& map);
	
private:
	//! init the StelSkyPolygon
	void initCtor();
	
	//! Return the list of tiles which should be drawn.
	//! @param result a map containing resolution, pointer to the tiles
	void getTilesToDraw(QMultiMap<double, StelSkyPolygon*>& result, StelCore* core, const SphericalRegionP& viewPortPoly, bool recheckIntersect=true);
	
	//! Draw the polygon on the screen.
	//! @return true if the tile was actually displayed
	bool drawTile(StelCore* core);
	
	//! Return the minimum resolution
	double getMinResolution() const {return minResolution;}

	// Used for smooth fade in
	QTimeLine* texFader;
};

#endif // _STELSKYPOLYGON_HPP_
