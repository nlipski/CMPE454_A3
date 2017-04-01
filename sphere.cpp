/* sphere.C
 */


#include "headers.h"

#include <math.h>
#include "sphere.h"
#include "main.h"
#include "texture.h"


// Ray / sphere intersection

bool Sphere::rayInt( vec3 rayStart, vec3 rayDir, int objPartIndex,
		     vec3 & intPoint, vec3 & intNorm, float & intParam, Material * &mat, int &intPartIndex )

{
  float a,b,c,d,t0,t1;

  // Does it intersect? ... Solve a quadratic for
  // the parameter at the point of intersection

  a = rayDir * rayDir;
  b = 2 * (rayDir * (rayStart - centre));
  c = (rayStart - centre) * (rayStart - centre) - radius * radius;

  d = b*b - 4*a*c;

  if (d < 0)
    return false;

  // Find the parameter at the point of intersection

  d = sqrt(d);
  t0 = (-b + d) / (2*a);
  t1 = (-b - d) / (2*a);

  intParam = (t0 < t1 ? t0 : t1);

  // Compute the point of intersection

  intPoint = rayStart + intParam * rayDir;

  // Compute the normal at the intersection point

  intNorm = (intPoint - centre).normalize();

  mat = this->mat;

  return true;
}


// Output a sphere

void Sphere::output( ostream &stream ) const

{
  stream << "sphere" << endl
	 << "  " << radius << endl
	 << "  " << centre << endl;
}


// Input a sphere

void Sphere::input( istream &stream )

{
  skipComments( stream );  stream >> radius;
  skipComments( stream );  stream >> centre;
}


// Render with openGL

#define PI          3.1415926
#define PHI_STEP    PI/8.0
#define THETA_STEP  PI/8.0

void Sphere::renderGL( GPUProgram *prog )

{
  for (float phi=0; phi<1.99*PI; phi+=PHI_STEP)
    for (float theta=-0.5*PI; theta<0.49*PI; theta+=THETA_STEP) {

      vec3 Nll = polarToCart( phi,          theta );
      vec3 Nlr = polarToCart( phi+PHI_STEP, theta );
      vec3 Nur = polarToCart( phi+PHI_STEP, theta+THETA_STEP );
      vec3 Nul = polarToCart( phi,          theta+THETA_STEP );

      vec3 ll = centre + radius * Nll;
      vec3 lr = centre + radius * Nlr;
      vec3 ur = centre + radius * Nur;
      vec3 ul = centre + radius * Nul;

      glBegin( GL_POLYGON );

      if (mat->texture != NULL)
	glTexCoord2f( phi / (2*PI), theta / PI + 0.5 );
      glNormal3fv( (GLfloat *) & Nll.x );
      glVertex3fv( (GLfloat *) & ll.x );

      if (mat->texture != NULL)
	glTexCoord2f( (phi+PHI_STEP) / (2*PI), theta / PI + 0.5 );
      glNormal3fv( (GLfloat *) & Nlr.x );
      glVertex3fv( (GLfloat *) & lr.x );

      if (mat->texture != NULL)
	glTexCoord2f( (phi+PHI_STEP) / (2*PI), (theta+THETA_STEP) / PI + 0.5 );
      glNormal3fv( (GLfloat *) & Nur.x );
      glVertex3fv( (GLfloat *) & ur.x );

      if (mat->texture != NULL)
	glTexCoord2f( phi / (2*PI), (theta+THETA_STEP) / PI + 0.5 );
      glNormal3fv( (GLfloat *) & Nul.x );
      glVertex3fv( (GLfloat *) & ul.x );

      glEnd();
    }
}


vec3 Sphere::polarToCart( float phi, float theta )

{
  return vec3( cos(phi)*cos(theta), sin(phi)*cos(theta), sin(theta) );
}


vec3 Sphere::textureColour( vec3 p, int objPartIndex, float &alpha )

{
  // No texture map?

  if (mat->texture == NULL) {
    alpha = 1;
    return vec3(1,1,1);
  }

  // Texture map:

  vec3 dir = (p - centre).normalize();

  float theta = asin(dir.z) / PI + 0.5;
  float phi = atan2( dir.y, dir.x ) / (2*PI);
  if (phi < 0) phi++;

  return mat->texture->texel( phi, theta, alpha );
}
