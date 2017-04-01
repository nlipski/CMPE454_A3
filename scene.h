/* scene.h
 */


#ifndef SCENE_H
#define SCENE_H


class RTwindow;


#include <iostream>
#include "seq.h"
#include "linalg.h"
#include "object.h"
#include "light.h"
#include "eye.h"
#include "material.h"
#include "photonmap.h"


class PhotonMap;


class Scene {

  RTwindow *    win;		// rendering window

  Eye *         eye;		// viewpoint
  seq<Light *>  lights;		// all lights
  seq<Object *> objects;	// all objects

  vec3        Ia;		// ambient illumination

  vec3  llCorner, up, right;	// window parameters

  seq<vec3> storedPoints;
  friend class PhotonMap;

 public:

  PhotonMap    photonMap;
  float        maxPhotonDist;
  int          maxPhotonCount;

  bool storingRays;
  seq<vec3> storedRays;	// each pair of points is a ray
  seq<vec3> storedRayColours;
  seq<Photon*> storedPhotons;

  seq<Texture*> textures;	// all textures
  seq<Material*> materials;	// all materials
  int maxDepth;			// ray tracing depth
  int glossyIterations;		// number of rays to send for glossy reflections
  bool useTextureTransparency;
  bool showAxes;
  bool showPhotonMap;

  Scene() {
    Ia = vec3(0.1,0.1,0.1);
    maxDepth = 3;
    glossyIterations = 3;
    useTextureTransparency = true;
    storingRays = false;
    showAxes = false;
    maxPhotonDist = 0.5;
    maxPhotonCount = 50;
    showPhotonMap = false;
  }

  void setWindow( RTwindow * w )
    { win = w; }

  void renderRT( bool restart );
  void renderGL();
  void read( const char *basename, istream &in );
  void write( ostream &out );
  vec3 pixelColour( int x, int y );
  vec3 raytrace( vec3 &rayStart, vec3 &rayDir, int depth, int thisObjIndex, int thisObjPartIndex );
  vec3 calcIout( vec3 N, vec3 L, vec3 E, vec3 R,
		   vec3 Kd, vec3 Ks, float ns, vec3 In );
  bool findFirstObjectInt( vec3 rayStart, vec3 rayDir, int thisObjIndex, int thisObjPartIndex,
			   vec3 &P, vec3 &N, float &param, int &objIndex, int &objPartIndex, Material *&mat );

  bool findRefractionDirection( vec3 &rayDir, vec3 &N, vec3 &refractionDir );

  void outputEye()
    { cout << *eye << endl; }

  void drawStoredRays();
  void drawAxes();
};


#endif
