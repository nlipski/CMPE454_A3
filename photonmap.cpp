// photonmap.cpp


#include <cstdlib>
#include "photonmap.h"
#include "scene.h"
#include "priority.h"
#include "priority.cpp"		// necessary to show implemenation to compiler


#define PHOTONS_PER_LIGHT 1000000// lots!


// Trace lots of rays outward in random directions from each light.
// Then build the KD tree.


void PhotonMap::populate( Scene *scene )

{
  clear();

  // Collect the photons

  // YOUR CODE HERE
  //
  // Call forwardTraceRay() for lots of directions around each point
  // light.  Use randomDir() for the directions.
   
	int i=0, j=0;	
	for ( i=0; i< scene->lights.size(); i++){
		for (j=0; j<PHOTONS_PER_LIGHT; j++){
			vec3 dir=randomDir();
			forwardTraceRay((scene->lights[i]->position),dir,scene->lights[i]->colour/PHOTONS_PER_LIGHT, i, j,scene, false, 0);
			
		}
		
	}
  // Build the KD tree

  
  if (photons.size() > 0) {
    tree = new KDTree();
    tree->buildFromPhotons( photons, c000, c111 ); // also computes bounding box and stores in c000, c111
  }

  cout << "KD tree built.  " << photons.size() << " photons captured." << endl;
}



// Pick a uniform random direction


vec3 PhotonMap::randomDir() 

{
  // YOUR CODE HERE
  //
  // Generate a *uniform* random direction
  float a,b,c,d;

      a=(((float)rand()/RAND_MAX))*2 -1.0;
      b=(((float)rand()/RAND_MAX))*2 -1.0;
      c=(((float)rand()/RAND_MAX))*2 -1.0;
      d=sqrt((a*a) + (b*b) +(c*c));




  return vec3(a,b,c);
}



// Trace a ray from the light


void PhotonMap::forwardTraceRay( vec3 &start, vec3 &dir, vec3 power, int thisObjIndex, int thisObjPartIndex, Scene *scene, bool specularDone, int depth )

{
  vec3 intPoint, intNorm;
  float t;
  int objIndex,objPartIndex;
  Material *mat;
  float prob;
  float s,d,ref_t,a;
  float Ks,Kd;

  // YOUR CODE HERE
  //
  // 1. Find first object hit by photon.
  //



if(!scene->findFirstObjectInt( start, dir, thisObjIndex, thisObjPartIndex, intPoint, intNorm, t, objIndex, objPartIndex, mat ))
  return;


  // 2. Store the photon if it has already has a specular reflection and if this is a diffuse surface.
  //
  if (specularDone==true &&  (mat->kd*mat->kd)>0){
      
     photons.add(new Photon(intPoint,  dir,power));
  }
  //3. Stop if at max depth.
  //


  if (depth>= scene->maxDepth){
        return;
  }
  // 4. Find the probability that the photon continues.
  //

  // 5. Generate random number.  If that shows that the photon dies, return.
  //
  //printf("%f\t%f\t%f",mat->ks.x,mat->ks.y,mat->ks.z);
  if ((mat->ks.x >= mat->ks.y) && (mat->ks.x >= mat->ks.z))
    Ks=mat->ks.x;
  else if((mat->ks.y >= mat->ks.z) && (mat->ks.y >= mat->ks.x))
    Ks=mat->ks.y;
  else
    Ks= mat->ks.z;

	Ks = max(mat->ks.x,mat->ks.y, mat->ks.z);


  if ((mat->kd.x >= mat->kd.y) && (mat->kd.x >= mat->kd.z))
    Kd=mat->kd.x;
  else if((mat->kd.y >= mat->kd.z) && (mat->kd.y >= mat->kd.x))
    Kd=mat->kd.y;
  else
    Kd= mat->kd.z;

	Kd = max(mat->kd.x,mat->kd.y, mat->kd.z);

  //printf("%f    %f     %f\n", intNorm.x,intNorm.y,intNorm.z );

  t= 1.0-mat->alpha;
  
  if(1.0-Ks-Kd-t>0) 
    prob= 1.0-Ks-Kd-t;
  else
    prob=0;

  float rand_res=(((float)rand()/RAND_MAX));
  vec3 random_dir =randomDir();
   vec3 refract_dir;
  
    random_dir =randomDir();
 //printf("%f\t%f\t%f\t%f\n", Ks, Kd, t, prob);

  // 6. Probabilistically determine the action (one of diffuse or
  //      specular reflection for all surfaces, plus refracted
  //      transmission for non-opaque surfaces).  Based on the action,
  //      compute the power loss (maybe!) and a direction to follow.
  //
  vec3 E =  (-1 * dir).normalize();
    vec3 R = (2 * (E * intNorm)) * intNorm- E;
   

    if(rand_res <= prob){//absorb
      return;
    }
    else if(rand_res>prob && rand_res <= (Ks + prob)){ //Specular
    specularDone=true;
    forwardTraceRay( intPoint, R, power, thisObjIndex, thisObjPartIndex, scene, specularDone, depth++ );
    }
    else if( rand_res>(Ks + prob) && rand_res <= (Ks + prob + Kd) ){//diffuse
       
	   forwardTraceRay( intPoint, random_dir, power, thisObjIndex, thisObjPartIndex, scene, specularDone, depth++ );
    }
    else{//refract
      if (scene->findRefractionDirection(dir,intNorm,refract_dir)){
      specularDone=true;
       forwardTraceRay( intPoint, refract_dir, power, thisObjIndex, thisObjPartIndex, scene, specularDone, depth+1 );
     }

    }



  
  // 7. Send the photon onward with recursive call to forwardTraceRay().
  //forwardTraceRay()
}



// ----------------------------------------------------------------
//
//  KD Tree
//
// ----------------------------------------------------------------



// Build the KD tree from a list of photons


void KDTree::buildFromPhotons( seq<Photon*> &photons, vec3 &min, vec3 &max )

{
  // Find the bounding box
  
  min = vec3(MAXFLOAT,MAXFLOAT,MAXFLOAT);
  max = vec3(-MAXFLOAT,-MAXFLOAT,-MAXFLOAT);
Photon *p;
  // YOUR CODE HERE
  //
  // Set min and max to the corners of the bounding box of all the photons.
  for(int i=0;i< photons.size();i++){
    p = photons[i];
    //printf("%f\t%f\n",min.x, max.x);
    if(p->pos.x < min.x)
      min.x = p->pos.x; 
    
    if(p->pos.y< min.y)
      min.y= p->pos.y;
    
    if(p->pos.z < min.z)
      min.z = p->pos.z;
    
    if(p->pos.x > max.x){
      max.x = p->pos.x;
      //printf("sausage\n");
    }
    if(p->pos.y > max.y){
      max.y = p->pos.y;
    }
    if(p->pos.z > max.z){
      max.z = p->pos.z;
    }

  }
  //printf("%f\t\n",photons[photons.size()-1]->pos.x);
  //printf("%f\t%f\n",min.x, max.x);


  // Build the tree
  
  root = root->buildSubtreeFromPhotons( &photons[0], photons.size(), min, max );
}



// Used as the 'qsort' comparison function to sort photon on one of
// the three axes (specified by 'splitDir' = 0,1,2 for x,y,z)


int comparePhotonPs( Photon **p1, Photon **p2, int *splitDir )

{
  short i = * (short*) splitDir;

  if ((*p1)->pos[i] < (*p2)->pos[i])
    return -1;
  else if ((*p1)->pos[i] > (*p2)->pos[i])
    return +1;
  else
    return 0;
}



// Given an array of n photons, and the min and max of the bounding
// box, return a pointer to a KD tree of the photons.


KDSubtree * KDSubtree::buildSubtreeFromPhotons( Photon **photons, int n, vec3 &min, vec3 &max )

{
  // Empty tree?
  if (n <= 0)
    return NULL;

  // YOUR CODE HERE
  //
  // 1. Choose the dimension along which to split.  Set 'splitDir' to 0 (for x), 1 (for y), or 2 (for z)
  //
  
  float xdist,ydist,zdist;
  xdist=max.x - min.x;
  ydist=max.y - min.y;
  zdist=max.z - min.z;
  short i;
 
 
  if (xdist>ydist && xdist> zdist)
    i=0;
  else if(ydist>xdist && ydist> zdist)
    i=1;
  else 
    i=2;
  
  // 2. Find the median photon in the split direction.  You can do
  //    this by sorting in O(n log n) (use 'qsort' on Unix) but in
  //    production code you would really do O(n) median finding.
  // 

  qsort_r(photons,n, sizeof(photons[0]), 
                  (int (*)(const void *, const void *, void *))comparePhotonPs,(void*)&i);
  // 3. Split the photons into two groups, construct the root, and
  //    recursively attach the two subtrees to the root.  Return the root.
  printf("%d\n",n );
  Photon *photon=photons[n/2];
  KDSubtree *left,*right;

  
  
  KDSubtree *root = new KDSubtree(i, photon, buildSubtreeFromPhotons(photons, n/2, min,photons[n/2]->pos ), buildSubtreeFromPhotons(photons, n/2, photons[n/2]->pos,max ));
     
     return root;

}



// Return a list of photons that are closest to 'pos'.  The list will
// contain the most distant photon first and the least distant photon
// last.  At most 'maxCount' photons are returned and all must be
// within 'maxSqDist' (max *squared* distance).
 

seq<Photon*> * KDTree::findNearest( vec3 &pos, float maxSqDist, int maxCount )

{
  // Use a priority queue (a heap) with the priority = the squared
  // distance from 'pos'.  The queue stores the max-priority element
  // at the top.

  priority_queue<Photon*> queue;

  // Find the closest photons
  //
  // Traverse the tree, going into those subtrees that are within
  // distance 'maxDist' of 'pos'

  if (root != NULL)
    root->findSubtreeNearest( pos, maxSqDist, maxCount, queue );

  // Package the result

  seq<Photon*> * photons = new seq<Photon*>( queue.n );

  while (!queue.empty())
    photons->add( queue.remove_max() );

  return photons;
}



// Within this subtree, collect the nearest photons


void KDSubtree::findSubtreeNearest( vec3 &pos, float maxSqDist, int maxCount, priority_queue<Photon*> &queue )

{
  // YOUR CODE HERE
  //
  // 1. If the photon in this subtree's root is close enough to 'pos', add it to the queue.
  if(maxCount == 0){
    return;
  }
    if (sqrt((pos-photon->pos)*(pos-photon->pos))<maxSqDist){      
      queue.add(photon,1);
      if (right!=NULL)
         right->findSubtreeNearest( pos, maxSqDist,  maxCount-1, queue );
      else if (left!=NULL )
          left->findSubtreeNearest( pos, maxSqDist,  maxCount-1, queue );
    }
  // 2. Recurse into the child subtree if 'pos' is within sqrt(maxSqDist) of that subtree's bounding box.
}




// ----------------------------------------------------------------
//
//   Drawing the photon map and KD tree
//
// ----------------------------------------------------------------



// Draw one subtree (as long as numLevels > 0)


void KDSubtree::draw( vec3 &min, vec3 &max, int numLevels )

{
  // Draw the partition

  vec3 c00, c10, c11, c01;

  vec3 &pos = photon->pos;

  // Find the partition corners
  
  switch (splitDir) {
  case 0: // X
    c00 = vec3( pos.x, min.y, min.z );
    c10 = vec3( pos.x, max.y, min.z );
    c11 = vec3( pos.x, max.y, max.z );
    c01 = vec3( pos.x, min.y, max.z );
    break;
  case 1: // Y
    c00 = vec3( min.x, pos.y, min.z );
    c10 = vec3( max.x, pos.y, min.z );
    c11 = vec3( max.x, pos.y, max.z );
    c01 = vec3( min.x, pos.y, max.z );
    break;
  case 2: // Z
    c00 = vec3( min.x, min.y, pos.z );
    c10 = vec3( max.x, min.y, pos.z );
    c11 = vec3( max.x, max.y, pos.z );
    c01 = vec3( min.x, max.y, pos.z );
    break;
  }

  // Draw the rectangular partition

  glBegin( GL_LINE_LOOP );
  glVertex3fv( &c00.x );
  glVertex3fv( &c10.x );
  glVertex3fv( &c11.x );
  glVertex3fv( &c01.x );
  glEnd();

  // Find the bounding boxes of the two subtrees

  numLevels--;

  if (numLevels > 0) {

    vec3 leftMax = max;
    vec3 rightMin = min;

    leftMax[splitDir] = pos[splitDir];
    rightMin[splitDir] = pos[splitDir];

    // Recursively draw the two subtrees

    if (left != NULL)
      left->draw( min, leftMax, numLevels );

    if (right != NULL)
      right->draw( rightMin, max, numLevels );
  }
}



// Draw the bounding box, then draw the tree


void KDTree::draw( vec3 &min, vec3 &max, int treeDisplayDepth )

{
  // Draw bounding box

  glColor3f( 0.6, 0.6, 0.6 );

  glBegin( GL_LINES );
  glVertex3f(min.x, min.y, min.z); glVertex3f(min.x, min.y, max.z);
  glVertex3f(min.x, max.y, min.z); glVertex3f(min.x, max.y, max.z);
  glVertex3f(max.x, min.y, min.z); glVertex3f(max.x, min.y, max.z);
  glVertex3f(max.x, max.y, min.z); glVertex3f(max.x, max.y, max.z);
  glVertex3f(min.x, min.y, min.z); glVertex3f(max.x, min.y, min.z);
  glVertex3f(min.x, min.y, max.z); glVertex3f(max.x, min.y, max.z);
  glVertex3f(min.x, max.y, min.z); glVertex3f(max.x, max.y, min.z);
  glVertex3f(min.x, max.y, max.z); glVertex3f(max.x, max.y, max.z);
  glVertex3f(min.x, min.y, min.z); glVertex3f(min.x, max.y, min.z);
  glVertex3f(min.x, min.y, max.z); glVertex3f(min.x, max.y, max.z);
  glVertex3f(max.x, min.y, min.z); glVertex3f(max.x, max.y, min.z);
  glVertex3f(max.x, min.y, max.z); glVertex3f(max.x, max.y, max.z);
  glEnd();

  // Draw the tree partitions

  if (root != NULL && treeDisplayDepth > 0)
    root->draw( min, max, treeDisplayDepth );
}



// Draw the photons and the KD tree


void PhotonMap::draw( seq<Photon*> &photonsToHighlight ) 

{
  // Draw partitions

  if (tree != NULL)
    tree->draw( c000, c111, treeDisplayDepth );

  // Draw photons with direction lines

  if (photons.size() > 0) {
    
    // Photons

    for (int i=0; i<photons.size(); i++) {
      
      if (photonsToHighlight.exists( photons[i] )) {
	glColor3f( 0.9, 0.7, 0.2 );
	glPointSize( 6.0 );
      } else {
	glColor3f( 0.7, 0.3, 0.1 );
	glPointSize( 3.0 );
      }
    
      glBegin( GL_POINTS );
      glVertex3fv( &photons[i]->pos.x );
      glEnd();

      glBegin( GL_LINES );
      vec3 head = photons[i]->pos + 0.03 * photons[i]->dir;
      glVertex3fv( &photons[i]->pos.x );
      glVertex3fv( &head.x );
      glEnd();
    }
  }
}


