#version 410

varying float depth;
flat in int instanceID;  
in vec4 colour;
in vec4 projPos;
out vec4 fragColour;

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
//    if (x < 0.125 && y < 0.125)
//      fragColour = vec4(50000.0, 50000.0, 50000.0, 1.0); //colour;
//    else
//      discard;
      fragColour = vec4(depth, depth, depth, 1); //colour;

    //fragColour = vec4(50000.0, 50000.0, 50000.0, 1.0); //colour;
    //fragColour = vec4(1, 1, 1, 1); //colour;
  }
}
