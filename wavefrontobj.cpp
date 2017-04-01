/* wavefrontobj.cpp
 */


#include "headers.h"
#include "wavefrontobj.h"
#include "material.h"


// Compute plane/ray intersection, and then the local coordinates to
// see whether the intersection point is inside.

bool WavefrontObj::rayInt( vec3 rayStart, vec3 rayDir, int objPartIndex,
			   vec3 & intPoint, vec3 & intNorm, float & intParam, Material * &mat, int &intPartIndex )

{
  intParam = MAXFLOAT;

  int partIndex = -1;

  for (int i=0; i<obj->groups.size(); i++)
    for (int j=0; j<obj->groups[i]->triangles.size(); j++) {

      partIndex++;

      if (partIndex != objPartIndex) {

	wfTriangle *tri = obj->groups[i]->triangles[j];
	vec3 &v0 = obj->vertices[ tri->vindices[0] ];
	vec3 &v1 = obj->vertices[ tri->vindices[1] ];
	vec3 &v2 = obj->vertices[ tri->vindices[2] ];
	vec3 &faceNormal = obj->facetnorms[ tri->findex ];

	// Adapted from triangle.cpp

	// Compute ray/plane intersection

	float dn = rayDir * faceNormal;

	if (fabs(dn) < 0.0001) 	// *** CHANGED TO ALLOW INTERSECTION FROM BEHIND TRIANGLE ***
	  continue;		// ray is parallel to plane

	float dist = faceNormal * v0;

	float param = (dist - rayStart*faceNormal) / dn;
	if (param < 0)
	  continue;		// plane is behind starting point
  
	vec3 point = rayStart + param * rayDir;

	// Compute barycentric coords

	float triSize = ((v1-v0) ^ (v2-v0)) * faceNormal;

	float beta  = (((v1-v0) ^ (point - v0)) * faceNormal) / triSize;
	float alpha = (((point-v0) ^ (v2 - v0)) * faceNormal) / triSize;
	float gamma = 1 - alpha - beta; 

	if (alpha < 0 || beta < 0 || gamma < 0)
	  continue;		// outside of triangle

	if (param < intParam) {	// found a new closest point
	
	  intParam = param;
	  intPoint = point;
	  mat = mats[i];		// this group's material
	  intPartIndex = partIndex;

	  // Find the normal with bump mapping

	  if (mat->bumpMap != NULL)
	    intNorm = faceNormal;	// NOT YET IMPLEMENTED!
	  else // No bump mapping: Find the normal with phong shading
	    if (!obj->hasVertexNormals)
	      intNorm = faceNormal;
	    else {
	      intNorm = gamma * obj->normals[ tri->nindices[0] ] + alpha * obj->normals[ tri->nindices[1] ] + beta * obj->normals[ tri->nindices[2] ];
	      intNorm = intNorm.normalize();
	    }
	}
      }
    }

  return (intParam != MAXFLOAT);
}


// Determine the texture colour at a point


vec3 WavefrontObj::textureColour( vec3 p, int objPartIndex, float &alpha )

{

  if (!obj->hasVertexTexCoords) { // no texture coordinates
    alpha = 1;
    return vec3(1,1,1);
  }

  // Find what group this part is in

  int groupStart;
  int partIndex = -1;
  int g;

  for (g=0; g<obj->groups.size(); g++) {
    groupStart = partIndex + 1;
    partIndex += obj->groups[g]->triangles.size();
    if (objPartIndex <= partIndex)
      break;
  }

  if (g == obj->groups.size()) {
    cerr << "Couldn't find part index " << objPartIndex << " in object " << obj->pathname << ", which has " << partIndex << " vertices." << endl;
    exit(1);
  }

  if (mats[g]->texture == NULL) { // no texture map
    alpha = 1;
    return vec3(1,1,1);
  }

  // Got a texture map

  wfTriangle *tri = obj->groups[g]->triangles[ objPartIndex - groupStart ];
  vec3 &v0 = obj->vertices[ tri->vindices[0] ];
  vec3 &v1 = obj->vertices[ tri->vindices[1] ];
  vec3 &v2 = obj->vertices[ tri->vindices[2] ];
  vec3 &faceNormal = obj->facetnorms[ tri->findex ];

  // Compute barycentric coords

  float triSize = ((v1-v0) ^ (v2-v0)) * faceNormal;

  float w = (((v1-v0) ^ (p - v0)) * faceNormal) / triSize;
  float v = (((p-v0) ^ (v2 - v0)) * faceNormal) / triSize;
  float u = 1 - v - w; 
  
  vec3 texCoords =
    u * obj->texcoords[ tri->tindices[0] ] +
    v * obj->texcoords[ tri->tindices[1] ] +
    w * obj->texcoords[ tri->tindices[2] ];

  vec3 texColour = mats[g]->texture->texel( texCoords.x, texCoords.y, alpha );

  return texColour;
}



// Copy materials from the wavefront object into this Object.  Convert
// from each wfMaterial to a Material.

void WavefrontObj::copyWfMaterialsToObject()

{
  for (int i=0; i<obj->groups.size(); i++) {

    wfMaterial *fromMat = obj->groups[i]->material;
    Material *toMat = new Material();

    toMat->name = fromMat->name;
    toMat->ka = fromMat->ambient;
    toMat->kd = fromMat->diffuse;
    toMat->ks = fromMat->specular;
    toMat->n  = fromMat->shininess;
    toMat->Ie = fromMat->emissive;
    toMat->alpha = fromMat->alpha;

    if (fromMat->texmap != NULL) {
      Texture *tex = new Texture(); // not used for OpenGL ... just for raytracing lookups
      tex->texmap             = fromMat->texmap;
      tex->width = tex->xdim  = fromMat->width;
      tex->height = tex->ydim = fromMat->height;
      tex->hasAlpha           = fromMat->hasAlpha;
      toMat->texture = tex;
    }

    // Not provided in wfMaterial:

    toMat->texName     = "";
    toMat->bumpMapName = "";
    toMat->g           = 1;
    toMat->alpha       = 1;

    // Add to Materials

    mats.add( toMat );
  }
}
