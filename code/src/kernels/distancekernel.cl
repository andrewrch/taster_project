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
  int imageX = get_global_id(0);
  int imageY = get_global_id(1);

  // The coordinates of the image patch we're working on
  int patchX = get_local_id(0);
  int patchY = get_local_id(1);
  int patchSizeX = width / get_local_size(0);
  int patchSizeY = height / get_local_size(1);

  int diffSum = 0, unSum = 0, intSum = 0;

  const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
  // Now iterate over each pixel in the patch
  for (int x = 0; x < patchSizeX; x++)
    for (int y = 0; y < patchSizeY; y++)
    {
      int2 rendCoords = (int2) (width * imageX + patchX * patchSizeX + x,
                                height + imageY + patchY * patchSizeY + y);
      int2 imageCoords = (int2) (patchX * patchSizeX + x,  patchY * patchSizeY + y);
      diffSum += (read_imageui(tiledRender, sampler, rendCoords).x -
                  read_imageui(depthImage, sampler, imageCoords).x);
      unSum += (read_imageui(tiledRender, sampler, rendCoords).x && \
                read_imageui(depthImage, sampler, imageCoords).x);
      intSum += (read_imageui(tiledRender, sampler, rendCoords).x && \
                 read_imageui(depthImage, sampler, imageCoords).x);


    }

    barrier(CLK_LOCAL_MEM_FENCE);
    differenceSum[imageX * numImagesInRow + imageY] += diffSum + 100;
    unionSum += unSum;
    intersectionSum += intSum;
    barrier(CLK_LOCAL_MEM_FENCE);
}
