__kernel void calculateImageCharacteristics (
                       __read_only image2d_t tiledRender,         // The tiled rendering
                       __read_only image2d_t skinImage,          // The skin image (segmented observation)
                       __read_only image2d_t depthImage,          // The depth image (raw observation)
                       __global unsigned int* differenceSum,      // Depth difference 
                       __global unsigned int* unionSum,           // Union of skin image and render
                       __global unsigned int* intersectionSum,    // intersection of skin image and render
                       unsigned int dm,                           // tolerance for difference in depths 
                       unsigned int dM,                           // Max depth for clamping
                       int numImagesInRow,
                       unsigned int width,
                       unsigned int height,
                       __local unsigned int* localDiff,
                       __local unsigned int* localUnion,
                       __local unsigned int* localInter)
{
  // The coordinates of the image we're working on
  int imageX = get_global_id(0) / get_local_size(0);
  int imageY = get_global_id(1) / get_local_size(1);

  // The coordinates of the image patch we're working on
  size_t patchX = get_local_id(0);
  size_t patchY = get_local_id(1);

  // The size of the patch in pixels (Should be 20x20)
  unsigned int patchSizeX = width / get_local_size(0);
  unsigned int patchSizeY = height / get_local_size(1);

  __private unsigned int unSum = 0, intSum = 0, diffSum = 0;

  const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;

  // Store these here so we dont do so much multiplication in the for loop
  int2 rCoordBase = (int2) (width * imageX + patchX * patchSizeX,
                            height * imageY + patchY * patchSizeY);
  int2 iCoordBase = (int2) (patchX * patchSizeX,  
                             patchY * patchSizeY);
  // Now iterate over each pixel in the patch
  for (int x = 0; x < patchSizeX; x++)
    for (int y = 0; y < patchSizeY; y++)
    {
      int2 rendCoords = (int2) (rCoordBase.x + x, rCoordBase.y + y);
      int2 imageCoords = (int2) (width - (iCoordBase.x + x), height - (iCoordBase.y + y));

      uint4 depthVal = read_imageui(depthImage, sampler, imageCoords);
      uint4 skinVal = read_imageui(skinImage, sampler, imageCoords);
      uint4 renderVal = convert_uint4(read_imagef(tiledRender, sampler, rendCoords));

//      if (skinVal.x != 0 && skinVal.x != 255 )
//      {
//        printf("Skin image...: %d %d %d %d %d \n", skinVal.x, depthVal.x, renderVal.x, imageCoords.x, imageCoords.y);
//      }
//      if (depthVal.x > 0 && cat)
//      {
//        printf("coords: %d %d, depth: %u\n", imageCoords.x, imageCoords.y, depthVal.x);
//        cat = false;
//      }

      int diff = abs_diff(depthVal.x, renderVal.x);
      int rm = (diff < dm) || depthVal.x == 0 ? 1 : 0;
      diffSum += (diff < dM) ? diff : dM;
      if (rm || skinVal.x > 0) unSum++;
      if (rm && skinVal.x > 0) intSum++;
    }

    unsigned int localPos = patchY * (width/patchSizeX) + patchX;
    localDiff[localPos] = diffSum;
    localUnion[localPos] = unSum;
    localInter[localPos] = intSum;

    // Now reduce over all local threads
    barrier(CLK_LOCAL_MEM_FENCE);
    if (patchX == 0 && patchY == 0)
    {
      unsigned int size = width/patchSizeX * height/patchSizeY;
      for (unsigned int i = 1; i < size; i++)
      {
        localDiff[0] += localDiff[i];
        localUnion[0] += localUnion[i];
        localInter[0] += localInter[i];    
      }

      unsigned int image = imageY * numImagesInRow + imageX;
      differenceSum[image] = localDiff[0];
      unionSum[image] = localUnion[0];
      intersectionSum[image] = localInter[0];
    }
    barrier(CLK_LOCAL_MEM_FENCE);
}
