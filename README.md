![alt text](https://carbonmade-media.accelerator.net/26384836;1916x1042.webp)

# CLFFT-TessendorfOcean
Real time Ocean Simulation based on Jerry Tessendorf’s paper ‘Simulating Ocean Waves’. The algorithm is based on a statistical model, in which wave height is a random variable of horizontal position and time. It decomposes the wave height field into a set of sine-waves with different amplitudes and phases. The model itself provides us with a method to generate these amplitudes and phases, and we use inverse Fast Fourier Transform as a means to quickly evaluate the sum of these sine-waves.

## Dependencies:

In order to build and run the demo, the following open-source libraries are needed:

- clFFT
- GLM
- OpenCL >= 1.2 (with cl_khr_gl_sharing extension)
- OpenGL >= 4.0
