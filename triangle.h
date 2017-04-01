/* triangle.h
 */


#ifndef TRIANGLE_H
#define TRIANGLE_H


#include "object.h"
#include "vertex.h"


class Triangle : public Object {

  Vertex verts[3];		// three vertices of the triangle
  vec3 faceNormal;		// triangle normal
  float  barycentricFactor;     // factor used in computing local coords
  float  dist;			// distance origin-to-plane of triangle

 public:

  Triangle() {}

  bool rayInt( vec3 rayStart, vec3 rayDir, int objPartIndex,
	       vec3 & intPoint, vec3 & intNorm, float & intParam, Material *&mat, int &intPartIndex );

  void input( istream &stream );
  void output( ostream &stream ) const;
  void renderGL( GPUProgram *prog );

  vec3 localCoords( vec3 point );
  vec3 barycentricCoords( vec3 point );
  vec3 textureColour( vec3 p, int objPartIndex, float &alpha );
  vec3 pointFromBarycentricCoords( float a, float b, float c );
};

#endif
