/* rtWindow.h
 */


#ifndef RTWINDOW_H
#define RTWINDOW_H

class RTwindow;


#include "arcballWindow.h"
#include "scene.h"


class RTwindow : public arcballWindow {

  Scene *scene;
  bool   viewpointChanged;

 public:

  RTwindow( int x, int y, int width, int height, const char *title, Scene *s )
    : arcballWindow( x, y, width, height, title ) {
    scene = s;
    viewpointChanged = true;
  }

  void draw() {
    viewpointChanged = true;
    scene->renderGL();
  }

  void raytrace() {
    scene->renderRT( viewpointChanged );
    viewpointChanged = false;
  }

  void mouseClick( float objX, float objY, int mouseX, int mouseY ) {

    scene->storedRays.clear();
    scene->storedRayColours.clear();
    scene->storedPhotons.clear();
    scene->storingRays = true;
    scene->pixelColour( mouseX, ydim-1-mouseY );
    scene->storingRays = false;
    glutPostRedisplay();
  }

  void userKeyAction( unsigned char key ) {
    switch (key) {
    case 127:
      scene->storedRays.clear();
      scene->storedRayColours.clear();
      glutPostRedisplay();
      break;
    case 27:
      exit(0);
    case 'e':
      scene->outputEye();
      break;
    case '+':
    case '=':
      scene->maxDepth++;
      viewpointChanged = true;
      break;
    case '-':
    case '_':
      if (scene->maxDepth > 1) {
	scene->maxDepth--;
	viewpointChanged = true;
      }
      break;
    case '<':
    case ',':
      if (scene->glossyIterations > 1) {
	scene->glossyIterations--;
	viewpointChanged = true;
      }
      break;
    case '>':
    case '.':
      scene->glossyIterations++;
      viewpointChanged = true;
      break;
    case 'm':
      if (scene->photonMap.isPopulated()) {
	cout << "cleared the photon map" << endl;
	scene->photonMap.clear();
      } else {
	cout << "building photon map" << endl;
	scene->photonMap.populate( scene );
	scene->showPhotonMap = true;
      }
      glutPostRedisplay();
      break;
    case 'M':
      scene->showPhotonMap = !scene->showPhotonMap;
      glutPostRedisplay();
      break;
    case '[':
      if (scene->photonMap.treeDisplayDepth > 0) {
	scene->photonMap.treeDisplayDepth--;
	glutPostRedisplay();
      }
      break;
    case ']':
      scene->photonMap.treeDisplayDepth++;
      glutPostRedisplay();
      break;
    case 'a':
      scene->showAxes = !scene->showAxes;
      glutPostRedisplay();
    }
  }
};


#endif
