Global Illumination and Photon Mapping

VC++2015

input: 

GlobalIllumination.exe -input scene14.txt -output output14.bmp -size 300 300 -samples 36 -bounces 4 

GlobalIllumination.exe -input scene15.txt -output output15.bmp -size 300 300 -samples 36 -bounces 4 

GlobalIllumination.exe -input scene18.txt -output output18.bmp -size 200 200 -bounces 10 -neighbors 10 -photonMultiplier 1 -PhotonMapping

output: creates or writes the output file. size is as input OR 3 times the size input on both dimensions for Photon Mapping. pretty slow. outputs progress messages.

condensed design notes: GI uses Ray Tracing in which secondary rays are from a continuous distribution, with importance sampling; luminous materials; soft shadows and caustics; Phong Materials only

PM has two phases. 1st is emitting photons (as Ray objects) from LightSource objects, interacting with objets, which depends on wavelength and spectral properties of the objects (and randomness)

2nd is gathering. Uses the k-Nearest Neighbors algorithm, which is not efficient for simple scenes, but I decided to try it anyways. 

Here is my sampling method for PM: for each Ray emitted, we bounce the Ray off all objects until it's absorbed, doesn't hit anything, or hits the max # bounces. We keep track of the points where it was not absorbed, tracked as a Departure object. Departures track incoming directions, the normal of the material there, the wavelength, and the material.

When gathering, we associate each Departure with a radiance, which corresponds to the radiance to the camera yielded by a ray of that wavelength, incoming direction, and normal according to the Phong model. 

The wavelength is an int 39-70, which is in 10nm. Refraction index and diffuse/absorb ratios depend on wavelength. Thus this can produce rainbows.

I've only drawn a really basic scene for rainbows so far. I am planning to add interpolation methods to speed up rendering, to eventually render a diamond.
