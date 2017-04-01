/* light.C
 */


#include "headers.h"

#include "light.h"
#include "main.h"


// Draw a light

void Light::draw()

{
  static GLUquadric *quadric = gluNewQuadric();

  glColor3f( 0.9, 0.82, 0.2 );
  
  glMatrixMode( GL_MODELVIEW );
  glPushMatrix();

  glTranslatef( position.x, position.y, position.z );
  gluQuadricOrientation( quadric, GLU_OUTSIDE );

  gluSphere( quadric, 0.1, 20, 20 ); 

  glPopMatrix();
}



// Output a light

ostream& operator << ( ostream& stream, Light const& obj )

{
  stream << "light" << endl
	 << "  " << obj.position << endl
	 << "  " << obj.colour << endl;
  return stream;
}


// Input a light

istream& operator >> ( istream& stream, Light & obj )

{
  skipComments( stream );  stream >> obj.position;
  skipComments( stream );  stream >> obj.colour;
  return stream;
}

