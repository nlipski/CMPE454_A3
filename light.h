/* light.h
 */


#ifndef LIGHT_H
#define LIGHT_H


#include "linalg.h"


class Light {

 public:

  vec3 position;
  vec3 colour;

  Light() {}

  void draw();

  friend ostream& operator << ( ostream& stream, Light const& obj );
  friend istream& operator >> ( istream& stream, Light & obj );
};

#endif
