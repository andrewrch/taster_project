__kernel void calculateImageCharacteristics (
                       __read_only image2d_t tiledRender, // The tiled rendering
                       __read_only image2d_t depthImage,  // The depth image (observation)
                       __global int* differenceSum,       // Depth difference 
                       __global int* unionSum,            // Union of skin image and render
                       __global int* intersectionSum,     // intersection of skin image and render
                       unsigned int dM,                   // Max depth for clamping
                       int numImagesInRow,
                       unsigned int width,
                       unsigned int height)
{
  // The coordinates of the image we're working on
  int imageX = get_global_id(0) / get_local_size(0);
  int imageY = get_global_id(1) / get_local_size(1);


  // The coordinates of the image patch we're working on
  int patchX = get_local_id(0);
  int patchY = get_local_id(1);

  //printf("%d %d %d %d\n", imageX, imageY, patchX, patchY); 
  //printf("%d %d\n", imageX / get_local_size(0), imageY / get_local_size(1));

  int patchSizeX = width / get_local_size(0);
  int patchSizeY = height / get_local_size(1);

  float diffSum = 0;
  int unSum = 0, intSum = 0;

  const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
  // Now iterate over each pixel in the patch
  for (int x = 0; x < patchSizeX; x++)
    for (int y = 0; y < patchSizeY; y++)
    {
      int2 rendCoords = (int2) (width * imageX + patchX * patchSizeX + x,
                                height * imageY + patchY * patchSizeY + y);
      int2 imageCoords = (int2) (patchX * patchSizeX + x,  
                                 patchY * patchSizeY + y);

//      printf("image coords: %d %d, renderCoords: %d %d\n", imageCoords.x, imageCoords.y, \
 //            rendCoords.x, rendCoords.y);


      uint4 renderVal = read_imageui(tiledRender, sampler, rendCoords);
      float4 depthVal = read_imagef(depthImage, sampler, imageCoords);


      if ( renderVal.x > 0 || renderVal.y > 0 || renderVal.z > 0)
      {
      printf("%f %f %f %f\n", depthVal.x * 65535, depthVal.y * 65535, depthVal.z * 65535, depthVal.w * 65535);
      printf("renderVal: %d %d %d %d\n", renderVal.x, renderVal.y, renderVal.z, renderVal.s3);
      //    printf("depthVal: %f %f %f %f\n", depthVal.x, depthVal.y, depthVal.z, depthVal.s3);
      }

      diffSum += (renderVal.x - depthVal.x);

      if (renderVal.x > 0 || depthVal.x > 0)
        unSum++;
      if (renderVal.x > 0 && depthVal.x > 0)
        intSum++;
    }

//    barrier(CLK_LOCAL_MEM_FENCE);
    differenceSum[imageX * numImagesInRow + imageY] += (int) diffSum + 100;
    unionSum[imageX * numImagesInRow + imageY] += unSum + 1;
    intersectionSum[imageX * numImagesInRow + imageY] += intSum + 2;
//    barrier(CLK_LOCAL_MEM_FENCE);
}
