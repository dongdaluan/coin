/**************************************************************************\
 *
 *  Copyright (C) 1998-2000 by Systems in Motion.  All rights reserved.
 *
 *  This file is part of the Coin library.
 *
 *  This file may be distributed under the terms of the Q Public License
 *  as defined by Troll Tech AS of Norway and appearing in the file
 *  LICENSE.QPL included in the packaging of this file.
 *
 *  If you want to use Coin in applications not covered by licenses
 *  compatible with the QPL, you can contact SIM to aquire a
 *  Professional Edition license for Coin.
 *
 *  Systems in Motion AS, Prof. Brochs gate 6, N-7030 Trondheim, NORWAY
 *  http://www.sim.no/ sales@sim.no Voice: +47 22114160 Fax: +47 67172912
 *
\**************************************************************************/

/*!
  \class SoPerspectiveCamera SoPerspectiveCamera.h Inventor/nodes/SoPerspectiveCamera.h
  \brief The SoPerspectiveCamera class defines a camera node with perspective rendering.
  \ingroup nodes

  For realistic looking 3D scene, the geometry should be rendered with
  perspective calculations. Use this camera type to accomplish this.
*/

#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/SbSphere.h>
#include <assert.h>

#if COIN_DEBUG
#include <Inventor/errors/SoDebugError.h>
#endif // COIN_DEBUG

/*!
  \var SoSFFloat SoPerspectiveCamera::heightAngle

  The vertical angle of the viewport, also known as "field of view".
  Default value is 45� (note: value is specified in radians).
*/

// *************************************************************************

SO_NODE_SOURCE(SoPerspectiveCamera);

/*!
  Constructor.
*/
SoPerspectiveCamera::SoPerspectiveCamera()
{
  SO_NODE_INTERNAL_CONSTRUCTOR(SoPerspectiveCamera);

  SO_NODE_ADD_FIELD(heightAngle, (float(M_PI)/4.0f));  // 45�.
}

/*!
  Destructor.
*/
SoPerspectiveCamera::~SoPerspectiveCamera()
{
}

// Doc in superclass.
void
SoPerspectiveCamera::initClass(void)
{
  SO_NODE_INTERNAL_INIT_CLASS(SoPerspectiveCamera);
}

/*!
  Scale the SoPerspectiveCamera::heightAngle field by multiplying with
  \a scalefactor.
*/
void
SoPerspectiveCamera::scaleHeight(float scalefactor)
{
  float tmp = this->heightAngle.getValue();
  this->heightAngle.setValue(tmp * scalefactor);
}

// Doc in superclass.
SbViewVolume
SoPerspectiveCamera::getViewVolume(float useaspectratio) const
{
  float angle = this->heightAngle.getValue();

  if (useaspectratio == 0.0f) useaspectratio = this->aspectRatio.getValue();
  else if (useaspectratio < 1.0f) {
    // FIXME: simplify? pederb, 20000402
    
    // use fovx = angle, and calculate fovy again
    float nearval = this->nearDistance.getValue();
    float xsize = nearval * float(tan(angle/2.0f));
    float ysize = xsize / useaspectratio;
    
    // now we can find the new fovy, and mortene owes me a beer, yay! :-)
    angle = 2.0f * float(atan(ysize / nearval));
  }
  
  SbViewVolume volume;
  volume.perspective(angle, useaspectratio,
                     this->nearDistance.getValue(),
                     this->farDistance.getValue());
  volume.rotateCamera(this->orientation.getValue());
  volume.translateCamera(this->position.getValue());
  return volume;
}

// Doc in superclass.
void
SoPerspectiveCamera::viewBoundingBox(const SbBox3f & box, float aspect,
                                     float slack)
{
#if COIN_DEBUG
  if (box.isEmpty()) {
    SoDebugError::postWarning("SoPerspectiveCamera::viewBoundingBox",
                              "bounding box empty");
    return;
  }
#endif // COIN_DEBUG

  // First, we want to move the camera in such a way that it is
  // pointing straight at the center of the scene bounding box -- but
  // without modifiying the rotation value (so we can't use
  // SoCamera::pointAt()).
  SbVec3f cameradirection;
  this->orientation.getValue().multVec(SbVec3f(0, 0, -1), cameradirection);
  this->position.setValue(box.getCenter() + -cameradirection);


  // Get the radius of the bounding sphere.
  SbSphere bs;
  bs.circumscribe(box);
  float radius = bs.getRadius();


  // Make sure that everything will still be inside the viewing volume
  // even if the aspect ratio "favorizes" width over height.
  float aspectradius = radius / (aspect < 1.0f ? aspect : 1.0f);

  // Move the camera to the edge of the bounding sphere, while still
  // pointing at the scene.
  SbVec3f direction = this->position.getValue() - box.getCenter();
  direction.normalize();
  float movelength =
    aspectradius + (aspectradius/float(atan(this->heightAngle.getValue())));
  this->position.setValue(box.getCenter() + direction * movelength);


  // Set up the clipping planes according to the slack value (a value
  // of 1.0 will yield clipping planes that are tangent to the
  // bounding sphere of the scene).
  float distance_to_midpoint =
    (this->position.getValue() - box.getCenter()).length();
  this->nearDistance = distance_to_midpoint - radius * slack;
  this->farDistance = distance_to_midpoint + radius * slack;


  // The focal distance is simply the distance from the camera to the
  // scene midpoint. This field is not used in rendering, its just
  // provided to make it easier for the user to do calculations based
  // on the distance between the camera and the scene.
  this->focalDistance = distance_to_midpoint;
}
