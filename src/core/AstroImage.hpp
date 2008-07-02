/*
 * Copyright (C) 2007 Fabien Chereau
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

#ifndef _ASTROIMAGE_HPP_
#define _ASTROIMAGE_HPP_

#include "STextureTypes.hpp"
#include "SphereGeometry.hpp"

class StelCore;
class ToneReproducer;

//! Base class for any astro image with a fixed position
class AstroImage
{
public:
	//! Constructor
	AstroImage();

	//! Create an AstroImage from a texture and the 4 corners of its projection in the sky
	//! The 4 corners can be given in any coordinate frame
	AstroImage(STextureSP tex, const Vec3d& v0, const Vec3d& v1, const Vec3d& v2, const Vec3d& v3);

	//! Create an empty AstroImage from the 4 corners of the image
	//! Call setTexture later to set its texture
	AstroImage(const Vec3d& v0, const Vec3d& v1, const Vec3d& v2, const Vec3d& v3);

	//! Desctructor
	virtual ~AstroImage();
	
	//! Draw the image on the screen. Assume that we are in Orthographic projection mode.
	void draw(StelCore* core);
	
	//! Return the ConvexPolygon describing the 4 corners of the sky-projected image
	const StelGeom::ConvexPolygon& getPolygon(void) const {return poly;}
	
	//! Get the texture of the image
	const STextureSP getTexture() const {return tex;}
	//! Set the texture for this image
	void setTexture(STextureSP atex) {tex=atex;}
	
private:
	friend class FitsAstroImage;
	
	// The texture matching the positions
	STextureSP tex;
	
	// Position of the 4 corners of the texture in sky coordinates
	const StelGeom::ConvexPolygon poly;
};

#endif // _ASTROIMAGE_H_
