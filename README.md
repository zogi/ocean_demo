Tessendorf wave simulation using OpenGL and OpenCL
==================================================

This demo renders a shaded ocean surface generated using the Tessendorf FFT
method \[1\].

The ocean surface extends to the horizon, which is achieved by projecting a
screen-space grid onto the ocean plane.
The projected grid points are then modified by the FFT-generated displacement
map.
This demo uses clFFT, so FFT computation runs on the GPU.

## Running the demo

The demo expects the kernels, shaders and textures directories
(along with their contents) to be present in the working directory
of the executable.
The textures directory has to contain a file named sky.png
(see textures/sky.txt).
One possible sky box which looks good can be found here (as of 20-Feb-2016):
http://mi.eng.cam.ac.uk/~agk34/resources/textures/sky/sky.png

The camera can be rotated by pressing the left mouse button and dragging.

## Settings

The settings, such as FFT size, projected grid size, MSAA sample count and
max texture anisotropy, are compiled in, an can be modified by editing src/main.cpp.

## Dependencies

In order to build and run the demo, the following open-source libraries are needed:

   * clFFT
   * DevIL
   * GLM (build only)
   * OpenCL >= 2.0 (with cl\_khr\_event and cl\_khr\_gl\_sharing extensions)
   * OpenGL >= 4.0 (with GL\_EXT\_texture\_filter\_anisotropic extension)
   * SDL2
   * SDL2\_ttf

---

\[1\] Tessendorf, J., 2001. Simulating ocean water. Simulating Nature: Realistic and Interactive Techniques. SIGGRAPH, 1(2), p.5.
