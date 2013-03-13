#version 330

varying float depth;
flat in int instanceID;  
in vec4 colour;
in vec4 projPos;
out ivec4 fragColour;

uniform float tileSize;
uniform unsigned int numPrimitives;
uniform unsigned int tilesPerRow;

void main()
{
  unsigned int tile = instanceID / numPrimitives;

  float x = ((projPos.x/projPos.w) + 1.0) / 2.0;
  float y = ((projPos.y/projPos.w) + 1.0) / 2.0;

  unsigned int tileY = tile / tilesPerRow;
  unsigned int tileX = tile % tilesPerRow;

  // Clip the hands if they are outside of the tile
  if (x < (tileX * tileSize) || y < (tileY * tileSize) ||
      x > ((tileX+1) * tileSize) || y > ((tileY+1) * tileSize))
    discard;
  // Otherwise colour with greyscale depth colour
  else
  {
    //float z = (projPos.z/projPos.w + 1.0f) / 2.0f;
    //float n = 40.0f; // camera z near
    //float f = 1000.0f; // camera z far
    //float z_e = (n * z) / ( f - z * (f - n) );

    //fragColour = vec4(z_e, z_e, z_e, 1.0); //colour;
    //
    // Use raw nonlinear depth like the Kinect.
    //fragColour = vec4(1.0 - z, 1.0 - z, 1.0 - z, 1.0); //colour;

    fragColour = ivec4(uint(depth), depth, depth, 1); //colour;
    //fragColour = vec4(50000 / 65535, 50000, 50000, 1); //colour;
  }
}
