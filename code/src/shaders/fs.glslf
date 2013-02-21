#version 330

flat in int instanceID;  
in vec4 colour;
in vec4 projPos;
out vec4 fragColour;

uniform float tileHeight;
uniform float tileWidth;
uniform unsigned int numPrimitives;
uniform unsigned int numTilesX;
uniform unsigned int numTilesY;

void main()
{
  unsigned int tile = instanceID / numPrimitives;

  float x = ((projPos.x/projPos.w) + 1.0) / 2.0;
  float y = ((projPos.y/projPos.w) + 1.0) / 2.0;

  unsigned int tileY = tile / numTilesX;
  unsigned int tileX = tile % numTilesX;

  // Clip the hands if they are outside of the tile
  if (x < (float(tileX) * tileWidth) || y < (float(tileY) * tileHeight) ||
      x > (float(tileX+1) * tileWidth) || y > (float(tileY+1) * tileHeight))
  {
    fragColour = vec4(1.0, 0.0, 0.0, 0.0);
  }
  // Otherwise colour with greyscale depth colour
  else
  {
    float z = (projPos.z/projPos.w + 1.0f) / 2.0f;
    float n = 1.0f; // camera z near
    float f = 100.0f; // camera z far
    float z_e = (n * z) / ( f - z * (f - n) );
    fragColour = vec4(1 - z_e, 1 - z_e, 1 - z_e, 1.0); //colour;
  }

}
