/* object.h
 */

#ifndef OBJECT_H
#define OBJECT_H


#include "linalg.h"
#include "material.h"
#include "gpuProgram.h"


class Object {

 public:

  Material *mat;

  Object() {}

  virtual bool rayInt( vec3 rayStart, vec3 rayDir, int objPartIndex,
		       vec3 & intPoint, vec3 & intNorm, float & intParam, Material * &mat, int &intPartIndex ) = 0;

  virtual vec3 textureColour( vec3 p, int objPartIndex, float &alpha ) {
    alpha = 1;
    return vec3(1,1,1);
  }

  virtual void renderGL( GPUProgram *prog ) {}

  virtual void input( istream &istream ) {}
  virtual void output( ostream &ostream ) const {}

  friend ostream& operator << ( ostream& stream, Object const& o );
  friend istream& operator >> ( istream& stream, Object & o );
};

#endif
