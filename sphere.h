/* sphere.h
 */


#ifndef SPHERE_H
#define SPHERE_H


#include "object.h"


class Sphere : public Object {

  vec3 centre;
  float radius;

 public:

  Sphere() {}

  bool rayInt( vec3 rayStart, vec3 rayDir, int objPartIndex,
	       vec3 & intPoint, vec3 & intNorm, float & intParam, Material * & mat, int &intPartIndex );

  void input( istream &stream );
  void output( ostream &stream ) const;
  vec3 polarToCart( float phi, float theta );
  vec3 textureColour( vec3 p, int objPartIndex, float &alpha );
  void renderGL( GPUProgram *prog );
};

#endif
