/* scene.C
 */


#include "headers.h"
#ifndef WIN32
  #include <unistd.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "scene.h"
#include "rtWindow.h"
#include "sphere.h"
#include "triangle.h"
#include "wavefrontobj.h"
#include "light.h"
#include "font.h"
#include "main.h"
#include "material.h"


#ifndef MAXFLOAT
  #define MAXFLOAT 9999999
#endif

void drawArrow( float radius, float len );


vec3 backgroundColour(1,1,1);
vec3 blackColour(0,0,0);


#define NUM_SOFT_SHADOW_RAYS 50


// Find the first object intersected

bool Scene::findFirstObjectInt( vec3 rayStart, vec3 rayDir, int thisObjIndex, int thisObjPartIndex,
				vec3 &P, vec3 &N, float &param, int &objIndex, int &objPartIndex, Material *&mat )

{
  if (storingRays)
    storedRays.add( rayStart );

  param = MAXFLOAT;

  for (int i=0; i<objects.size(); i++) {

    WavefrontObj* wfo = dynamic_cast<WavefrontObj*>( objects[i] );

    if (wfo || i != thisObjIndex) {	// don't check for int with the originating object
      vec3 point, normal;
      Material *intMat;
      float t;
      int intPartIndex;

      if (objects[i]->rayInt( rayStart, rayDir, ((i != thisObjIndex) ? -1 : thisObjPartIndex), point, normal, t, intMat, intPartIndex ))

      //printf("%f\n",t);
	if (0 < t && t < param) {
	  param = t;
	  P = point;
	  N = normal;
	  objIndex = i;
	  objPartIndex = intPartIndex;
	  mat = intMat;
	}
    }
  }
    
  if (storingRays) {

    // Is this ray aiming for a light?

    bool foundLight = false;
    
    for (int j=0; j<lights.size(); j++) {
      float lightToRayDist = ((lights[j]->position - rayStart) - ((lights[j]->position - rayStart) * rayDir) * rayDir).length();
      if (lightToRayDist < 0.001) {
	foundLight = true;
	break;
      }
    }

    if (param != MAXFLOAT) {
      storedRays.add( P );
      if (foundLight)
	storedRayColours.add( vec3(.9,.3,.1) ); // RED: shadow ray toward a light
      else
	storedRayColours.add( vec3(.1,.7,.7) ); // CYAN: normal ray that hits something
    } else {
      storedRays.add( rayStart+2*rayDir );
      if (foundLight)
	storedRayColours.add( vec3(.9,.3,.1) ); // RED: shadow ray toward a light
      else
	storedRayColours.add( vec3(.3,.3,.3) ); // GREY: normal ray that misses
    }

    // Collect photons at each intersection point

    if (param != MAXFLOAT && !foundLight) {
      seq<Photon*> *photons = photonMap.findNearest( P, maxPhotonDist*maxPhotonDist, maxPhotonCount );
      for (int i=0; i<photons->size(); i++)
	storedPhotons.add( (*photons)[i] );
    }
  }

  return (param != MAXFLOAT);
}

// Raytrace: This is the main raytracing routine which finds the first
// object intersected, performs the lighting calculation, and does
// recursive calls.
//
// This returns the colour received on the ray.

vec3 Scene::raytrace( vec3 &rayStart, vec3 &rayDir, int depth, int thisObjIndex, int thisObjPartIndex )

{
  // Terminate the ray?

  depth++;

  if (depth > maxDepth)
    return blackColour;

  // Find the closest object intersected

  vec3 P, N;
  float  t;
  int    objIndex, objPartIndex;
  Material *mat;

  findFirstObjectInt( rayStart, rayDir, thisObjIndex, thisObjPartIndex, P, N, t, objIndex, objPartIndex, mat );

  // No intersection: Return background colour

  if (t == MAXFLOAT)
    if (depth == 1)
      return backgroundColour;
    else
      return blackColour;

  // Find reflection direction & incoming light from that direction

  Object &obj = *objects[objIndex];

  vec3 E = (-1 * rayDir).normalize();
  vec3 R = (2 * (E * N)) * N - E;

  float  alpha;
  vec3 colour = obj.textureColour( P, objPartIndex, alpha );
  vec3 kd = vec3( colour.x*mat->kd.x, colour.y*mat->kd.y, colour.z*mat->kd.z );

  vec3 Iout = mat->Ie + vec3( mat->ka.x * Ia.x, mat->ka.y * Ia.y, mat->ka.z * Ia.z );

  // Compute glossy reflection

  if (mat->g < 0 || mat->g > 1) {
    cerr << "Material glossiness is outside the range [0,1]" << endl;
    exit(1);
  }

  float g = mat->g;

  if (g == 1 || glossyIterations == 1) {

    vec3 Iin = raytrace( P, R, depth, objIndex, objPartIndex );

    Iout = Iout + calcIout( N, R, E, E, kd, mat->ks, mat->n, Iin );

  } else if (g > 0) {

    // YOUR CODE HERE
    //
    // Add glossy reflection to Iout.  Use 'glossyIterations' samples
    // and use 'g' for the spread.
	vec3 Iin;
	float angle= acos(g);
  float a,b,z,c=2;
  float d=1/(tan(angle));
  vec3 u,v;
  vec3 p,plane_n;
  vec3 len,ray_dir;
	for (int i=0; i<glossyIterations;i++){

    while (c>1){
      a=(((float)rand()/RAND_MAX));
      b=(((float)rand()/RAND_MAX));
      c=(a*a)+(b*b);
     // std::cout<<a<<"    "<<b<<std::endl;
    }


   //p= P + (d*R);
    len=p-P;
    plane_n=len.normalize();
    
    z= plane_n * p;
    u = (vec3(0,0,z/plane_n.z)-p).normalize();
    v= u ^ plane_n;
    p= P + (d*R) + (a*u)+ (b*v);
    ray_dir=(p-P).normalize();
	  Iin = raytrace(P,ray_dir, depth, objIndex, objPartIndex);
	  Iout = Iout +  calcIout(N, R, E, R, kd, mat->ks, mat->n ,Iin)/glossyIterations;
	}	
      

  }

  // Add direct contributions from point lights

  for (int i=0; i<lights.size(); i++) {
    Light &light = *lights[i];

    vec3 L = light.position - P;

    if (N*L > 0) {
      float  Ldist = L.length();
      L = (1.0/Ldist) * L;

      vec3 intP, intN;
      float intT;
      int intObjIndex, intObjPartIndex;
      Material *intMat;

      // Is there an object between P and the light?

      bool found = findFirstObjectInt( P, L, objIndex, objPartIndex, intP, intN, intT, intObjIndex, intObjPartIndex, intMat );

      if (!found || intT > Ldist) { // no object: Add contribution from this light
	vec3 Lr = (2 * (L * N)) * N - L;
	Iout = Iout + calcIout( N, L, E, Lr, kd, mat->ks, mat->n, light.colour);
      }
    }
  }

  // Add contribution from photon map

  if (photonMap.isPopulated()) {
    //seq<Photon*> photons_map = findNearest()
    //printf("sausage\n");
    // YOUR CODE HERE
    /*
    Photon *p;
    for(int i =0; i< photonMap.photons.size(); i ++){
    p = photonMap.photons[i];
    vec3 Lp = p->pos - P;

      Iout = Iout + calcIout( N, Lp, p->dir, p->dir, kd, mat->ks, mat->n, p->power);

    }
    */
 
    // Iout = Iout + calcIout( N, L, E, Lr, kd, mat->ks, mat->n, light.colour);
    // Look up photons in the photon map and add their contribution to
    // Iout.  Note that the photon map is available only if you
    // pressed 'm' to populate it.

    
  }

  // Add transmission through non-opaque surfaces (with refraction!)

  float opacity = alpha * mat->alpha;

  if (opacity < 1) { // not completely opaque

    // YOUR CODE HERE
    //
    // Blend the light bounced from above the surface (= Iout) with
    // the light coming from below the surface (which you compute).
    // Blend according to 'opacity'.
    //
    // You should modify Scene::findRefractionDirection() and use it.
    vec3 refractionDir; 
    if(findRefractionDirection(rayDir, N, refractionDir))
       Iout = Iout*(opacity) + (1-opacity)*raytrace(P,refractionDir, depth, objIndex, objPartIndex);
    
  }

  return Iout;
}



// Find the refraction direction of a ray entering a surface in
// direction 'rayDir'.  The surface has normal 'N'.  Assume that under
// the surface is glass (index of refraction n = 1.510) and above the
// surface is air (n = 1.008).  Return false iff ray does not go
// through the surface (i.e. total internal reflection).


bool Scene::findRefractionDirection( vec3 &rayDir, vec3 &N, vec3 &refractionDir )

{
  // YOUR CODE HERE
  double n1, n2;
  double eta,c1,c2;
  double cosI = (N.normalize()*rayDir.normalize());

  vec3 E = (-1 * rayDir).normalize();
  vec3 R = (2 * (E * N)) * N - E;
 // float dot = rayDir*N;
  if (cosI>0){
    n1 =1.510;
    n2 =1.008;
    
    }
  else {

    n1 =1.008;
    n2 = 1.510;
    cosI = -1*cosI;

  } 
  double ratio = n1/n2;
  double sinT2 = ratio*sin(acos(cosI));
  double cosT = sqrt(1.0 - sinT2*sinT2);


  //float k =1.0- ratio*ratio *( 1.0 - dot*dot);
 if(sinT2 > 1.0){
    //refractionDir =R;
    return false;
  }

  else{
    refractionDir = (ratio * cosI -  cosT ) * N.normalize() + ratio*rayDir.normalize();
    return true;
  }


}



// Calculate the outgoing intensity due to light Iin entering from
// direction L and exiting to direction E, with normal N.  Reflection
// direction R is provided, along with the material properties Kd, 
// Ks, and n.
//
//       Iout = Iin * ( Kd (N.L) + Ks (R.V)^n )

vec3 Scene::calcIout( vec3 N, vec3 L, vec3 E, vec3 R,
			vec3 Kd, vec3 Ks, float ns,
			vec3 In )

{
  // Don't illuminate from the back of the surface

  if (N * L <= 0)
    return blackColour;

  // Both E and L are in front:

  vec3 Id = (L*N) * In;

  vec3 Is;

  if (R*E < 0)
    Is = blackColour;		// Is = 0 from behind the
  else				// reflection direction
    Is = pow( R*E, ns ) * In;

  return vec3( Is.x*Ks.x+Id.x*Kd.x, Is.y*Ks.y+Id.y*Kd.y, Is.z*Ks.z+Id.z*Kd.z );
}


// Determine the colour of one pixel.  This is where
// the raytracing actually starts.

vec3 Scene::pixelColour( int x, int y )

{
  vec3 dir    = (llCorner + x * right + y * up).normalize();

  vec3 result = raytrace( eye->position, dir, 0, -1, -1 );

  if (storingRays)
    storingRays = false;

  return result;
}


// Read the scene from an input stream

void Scene::read( const char *basename, istream &in )

{
  char command[1000];

  while (in) {

    skipComments( in );
    in >> command;
    if (!in || command[0] == '\0')
      break;
    
    skipComments( in );

    if (strcmp(command,"sphere") == 0) {

      Sphere *o = new Sphere();
      in >> *o;
      objects.add( o );
      
    } else if (strcmp(command,"triangle") == 0) {

      Triangle *o = new Triangle();
      in >> *o;
      objects.add( o );
      
    } else if (strcmp(command,"material") == 0) {

      Material *m = new Material();
      in >> *m;
      materials.add( m );
      
    } else if (strcmp(command,"wavefront") == 0) {

      // Rely on the wavefront.cpp code to read this

      string filename;
      in >> filename;
      WavefrontObj *o = new WavefrontObj( basename, filename.c_str() );
      objects.add( o );
      
    } else if (strcmp(command,"light") == 0) {

      Light *o = new Light();
      in >> *o;
      lights.add( o );
      
    } else if (strcmp(command,"eye") == 0) {

      eye = new Eye();
      in >> *eye;

      win->eye = eye->position;
      win->lookat = eye->lookAt;
      win->updir = eye->upDir;
      win->fovy = eye->fovy * 180.0/3.1415926;
      
    } else {
      
      cerr << "Command '" << command << "' not recognized" << endl;
      exit(-1);
    }
  }
}



// Output the whole scene (mainly for debugging the reader)


void Scene::write( ostream &out )

{
  out << *eye << endl;

  for (int i=0; i<lights.size(); i++)
    out << *lights[i] << endl;

  for (int i=0; i<materials.size(); i++)
    out << *materials[i] << endl;

  for (int i=0; i<objects.size(); i++)
    out << *objects[i] << endl;
}


// Draw the scene.  This sets things up and simply
// calls pixelColour() for each pixel.

void Scene::renderRT( bool restart )

{
  static bool stop = true;
  static float nextDot;
  static int nextx, nexty;

  if (restart) {

    // Copy the window eye into the scene eye

    eye->position = win->eye;
    eye->lookAt = win->lookat;
    eye->upDir = win->updir;
    eye->fovy = win->fovy * 3.1415926/180.0 ;

    // Set up the window

    glDisable( GL_DEPTH_TEST );
    glDisable( GL_LIGHTING );
    glDisable( GL_TEXTURE_2D );
    glDisable( GL_COLOR_MATERIAL );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( 0, win->xdim-0.99, 0, win->ydim-0.99, 0, 1 );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    // Compute the image plane coordinate system

    up = (2 * tan(eye->fovy/2.0)) * eye->upDir.normalize();

    right = (2 * tan(eye->fovy/2.0) * (float) win->xdim / (float) win->ydim)
      * ((eye->lookAt - eye->position) ^ eye->upDir).normalize();
  
    llCorner = (eye->lookAt - eye->position).normalize()
      - 0.5 * up - 0.5 * right;

    up = (1.0 / (float) (win->ydim-1)) * up;
    right = (1.0 / (float) (win->xdim-1)) * right;

    if (nextDot != 0) {
      cout << "\r           \r";
      cout.flush();
    }

    nextx = 0;
    nexty = 0;
    nextDot = 0;

    stop = false;
  }

  if (stop) {
#ifndef WIN32
    usleep(1000);
#endif
    return;
  }

  // Draw the next pixel

  vec3 colour = pixelColour( nextx, nexty );

  glBegin( GL_POINTS );
  glColor3fv( (GLfloat *) &colour.x );
  glVertex3i( nextx, nexty, 0 );
  glEnd();

  // Move (nextx,nexty) to the next pixel

  nexty++;
  if (nexty >= win->ydim) {
    nexty = 0;
    nextx++;
    if ((float)nextx/(float)win->xdim >= nextDot) {
      cout << "."; cout.flush(); nextDot += 0.1;
    }
    if (nextx >= win->xdim) {

      // Show status message
      
      char buffer[1000];
      sprintf( buffer, "depth %d", maxDepth );
      glColor3f(1,1,1);
      printString( buffer, 10, 10, win->xdim, win->ydim );

      // Show the photon map
      
      win->setupProjection();

      if (showPhotonMap)
	photonMap.draw( storedPhotons );

      // Show the lights

      for (int i=0; i<lights.size(); i++)
	lights[i]->draw();
      
      // Show the stored rays
      
      glPolygonMode( GL_FRONT, GL_FILL );
      glPolygonMode( GL_BACK, GL_LINE );
      drawStoredRays();

      // Axes

      if (showAxes)
	drawAxes();

      // Done
      
      glFlush();
      glutSwapBuffers();
      nextx = 0;
      stop = true;
      cout << "\r           \r";
      cout.flush();
    }
  }
}


// Render the scene with OpenGL


void Scene::renderGL()

{
  // Copy the window eye into the scene eye

  eye->position = win->eye;
  eye->lookAt = win->lookat;
  eye->upDir = win->updir;
  eye->fovy = win->fovy * 3.1415926/180.0 ;

  // Set up the framebuffer

  glClearColor( backgroundColour.x, backgroundColour.y, backgroundColour.z, 1 );
  
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  glEnable( GL_DEPTH_TEST );

  // Axes

  if (showAxes)
    drawAxes();

  // Draw any stored points (for debugging)

  glPointSize( 4.0 );
  glBegin( GL_POINTS );
  for (int i=0; i<storedPoints.size(); i++)
    glVertex3fv( (GLfloat*) &storedPoints[i].x );
  glEnd();

  // Show the lights

  for (int i=0; i<lights.size(); i++)
    lights[i]->draw();
      
  // Draw any stored rays (for debugging)

  drawStoredRays();

  // Show the photon map
  
  if (showPhotonMap)
    photonMap.draw( storedPhotons );

  // Set up the lights

  for (int i=0; i<lights.size(); i++) {

    GLfloat p[4];

    p[0] = lights[i]->position.x;
    p[1] = lights[i]->position.y;
    p[2] = lights[i]->position.z;
    p[3] = 0;
    glLightfv( (GLenum) (GL_LIGHT0+i), GL_POSITION, &p[0] );

    p[0] = lights[i]->colour.x;
    p[1] = lights[i]->colour.y;
    p[2] = lights[i]->colour.z;
    glLightfv( (GLenum) (GL_LIGHT0+i), GL_DIFFUSE,  &p[0] );

    p[0] = p[1] = p[2] = 0;
    glLightfv( (GLenum) (GL_LIGHT0+i), GL_AMBIENT,  &p[0] );

    p[0] = p[1] = p[2] = 1;
    glLightfv( (GLenum) (GL_LIGHT0+i), GL_SPECULAR, &p[0] );

    glEnable( (GLenum) (GL_LIGHT0+i) );
  }

  glShadeModel( GL_SMOOTH );

  GLfloat amb[4] = {0.0,0.0,0.0,0.0};
  glLightModelfv( GL_LIGHT_MODEL_AMBIENT, &amb[0] );

  glEnable( GL_LIGHTING );

  // Draw the objects

  for (int i=0; i<objects.size(); i++) {

    WavefrontObj* wfo = dynamic_cast<WavefrontObj*>( objects[i] );

    if (wfo) {

      // Wavefront object

      gpuProg->activate();

      mat4 MV = lookat( eye->position, eye->lookAt, eye->upDir );
      gpuProg->setMat4( "MV", MV );

      mat4 MVP = perspective( eye->fovy, win->xdim/(float)win->ydim, 1, 1000 )
	* MV;

      gpuProg->setMat4( "MVP", MVP );

      // Light direction for OpenGL rendering

      vec3 lightDir(2,0.5,0.6);
      lightDir = lightDir.normalize();

      gpuProg->setVec3( "lightDir", lightDir );

      objects[i]->renderGL( gpuProg );

      gpuProg->deactivate();

    } else {

      // Normal objects

      objects[i]->mat->setMaterialForOpenGL();
      objects[i]->renderGL( NULL );
    }
  }
}



// Draw an arrow up the z axis.  Length is about one unit.

void drawArrow( float radius, float len ) 

{
  static GLUquadric *quadric = gluNewQuadric();

  glMatrixMode( GL_MODELVIEW );
  glPushMatrix();

  glTranslatef( 0, 0, len-radius*10 );
  gluQuadricOrientation( quadric, GLU_INSIDE );
  gluDisk( quadric, 0, 2*radius, 20, 1 );

  glTranslatef( 0, 0, -(len-radius*10) );
  gluQuadricOrientation( quadric, GLU_OUTSIDE );
  gluCylinder( quadric, radius, radius, len-radius*10, 20, 20 );

  glTranslatef( 0, 0, len-radius*10 );
  gluQuadricOrientation( quadric, GLU_OUTSIDE );
  gluCylinder( quadric, 2*radius, 0, radius*10, 20, 20 );

  glPopMatrix();
}


// Draw any stored rays (for debugging)

void Scene::drawStoredRays()

{
  if (storedRays.size() > 0) {
	
    win->setupProjection();

    glEnable( GL_CULL_FACE );

    // Set up the light

    glPushMatrix();
    glLoadIdentity();

    GLfloat p[4];

    p[0] = 1;
    p[1] = 1;
    p[2] = 1;
    p[3] = 0;
    glLightfv( (GLenum) (GL_LIGHT0), GL_POSITION, &p[0] );

    p[0] = p[1] = p[2] = 1;
    glLightfv( (GLenum) (GL_LIGHT0), GL_DIFFUSE,  &p[0] );
    glLightfv( (GLenum) (GL_LIGHT0), GL_SPECULAR, &p[0] );

    p[0] = p[1] = p[2] = 0.5;
    glLightfv( (GLenum) (GL_LIGHT0), GL_AMBIENT, &p[0] );

    glEnable( (GLenum) (GL_LIGHT0 ) );
    glPopMatrix();

    glEnable( GL_LIGHTING );
    glEnable( GL_COLOR_MATERIAL );

    // Draw rays

    for (int i=0; i<storedRays.size(); i+=2) {

      glColor3fv( (GLfloat*) &storedRayColours[i/2].x );

      vec3 arrow = storedRays[i] - storedRays[i+1];
      mat4 xform = transpose(translate( storedRays[i+1] ) * rotate( vec3(0,0,1), arrow ));

      glPushMatrix();
      glMultMatrixf( &xform[0][0] );
      drawArrow(0.01, arrow.length()*0.98);
      glPopMatrix();
    }

    // Turn off the lights

    glDisable( GL_COLOR_MATERIAL );
    glDisable( GL_LIGHTING );
  }
}



void Scene::drawAxes() 

{
  static vec3 verts[] = {
    vec3(0,0,0), vec3(1,0,0),
    vec3(0,0,0), vec3(0,1,0),
    vec3(0,0,0), vec3(0,0,1)
  };

  static vec3 colours[] = {
    vec3(1,0,0), vec3(1,0,0),
    vec3(0,1,0), vec3(0,1,0),
    vec3(0.3,0.3,1), vec3(0.3,0.3,1)
  };

  glLineWidth( 3.0 );

  glBegin( GL_LINES );
  for (int i=0; i<6; i++) {
    glColor3fv( &colours[i][0] );
    glVertex3fv( &verts[i][0] );
  }
  glEnd();

  glLineWidth( 1.0 );
}
