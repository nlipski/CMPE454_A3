// Phong fragment shader with textures
//
// Phong shading with diffuse and specular components

#version 300 es

uniform sampler2D objTexture;
uniform bool      texturing;        // =1 if object's texture is provided

uniform mediump vec3 lightDir;

uniform mediump vec3 kd;
uniform mediump vec3 ks;
uniform mediump vec3 Ia;
uniform mediump vec3 Ie;
uniform mediump float shininess;

smooth in mediump vec3 normal;
smooth in mediump vec2 texCoords;

out mediump vec4 outputColour;

void main()

{
  mediump vec3 Iin = vec3( 1.0, 1.0, 1.0 ); // incoming light

  mediump vec3 N = normalize( normal );

  mediump float NdotL = dot( N, lightDir );

  // Compute the outgoing colour

  mediump vec3 Iout = Ia;

  if (NdotL > 0.0) {

    // diffuse component

    if (texturing)
      Iout += NdotL * (texture( objTexture, texCoords ).rgb * Iin);
    else
      Iout += NdotL * Iin;

    // specular component

    mediump vec3 R = (2.0 * NdotL) * N - lightDir;
    mediump vec3 V = vec3(0.0,0.0,1.0);	// direction toward eye in the VCS

    mediump float RdotV = dot( R, V );

    if (RdotV > 0.0)
      Iout += pow( RdotV, shininess ) * (ks * Iin);
  }

  outputColour = vec4( Iout, 1.0f );
}
