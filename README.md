# Graphics

Bobby Shen (Robert)

Note: There are shorter READMEs in each folder which give input and some condensed notes.

Here are some graphics projects I have worked on, written in MSVC++ 2010 or 2015. Currently, this majority of this repository is the assignments from the 6.837 course on MIT's OCW (from Fall 2012). The 'starter code' for these assignments consists of many structural elements or algorithms that deal with hardware, such as the structure of an OpenGL program, an arcball camera, some parsers, some class headers, some complete vector and matrix classes, object/scene descriptions in text, an acceleration structure, and so on. See http://ocw.mit.edu/courses/electrical-engineering-and-computer-science/6-837-computer-graphics-fall-2012/assignments/ (VC++ 2010, specifically). I did not change the started code except to set (0,0,0) normalized equal to (0,0,1) in some places (See Vector3f::normalized()). I have completed the rest myself. I decided not to upload assignments 0 and 1.

In general, my comments use ///, and the comments from the starter code use //. However, I commonly comment out test lines with // or /**/. Some projects may require one to disable incremental linking. Every project is in VC++ 2015 except SSD. To directly run an .exe, move it from the release/debug folder into the main folder. Alternatively, debug in Visual Studio, and recall that the one should not rewrite the file name as command line arguments in the project properties. Some of the programs stay open with a cin>>(int). Just enter anything.

The files a2soln.exe a3soln.exe a4soln.exe are sample solutions from the OCW (with no source code)

Some specifics of each project:

<b>SSD</b>: Hierarchical modeling and Skeletal Subspace Deformation. The FLTK parts seem to be incompatible with VC++ 2015.

Input: Command line arguments: SSD.exe data/ModelN (2 arguments), where N is the digit 1, 2, 3, or 4. This loads the corresponding SSD model. Use the sliders to define the Euler Angles to update each model. The z rotation is applied first, then y, then x. While in the OpenGL window, use the mouse to move the arcball camera: L Click + drag to rotate, R Click + drag to dilate about the origin, Both Click + drag to translate the objects and axes. Press s to toggle between drawing the skin and the skeleton.

Design notes: Uses SSD and triangles with flat normals so that there is a faceted appearance. Doesn't handle extreme rotations or collisions at all.

The file types .skel .attach .obj are just fancy types of text files.

There are three main types of spaces: (Current) World Space, Joint Space, and Bind World Space. 

World Space: what the skeleton looks like in the world given the current state. 

Joint Space: A local coordinate system in which the joint is at the origin. Points on the skin which are exclusively attached to that joint have a constant location in joint space. For example, the forearm has a constant location in the joint space of the elbow. The Joint to World Transform is a function of the current state (of rotations) and is calculated recursively after each input update, using my own matrix stack. The transform from one node's joint space to its parent is a function of the length (fixed) and the input angles. Both are stored with the joint.

Bind Space: The location of the skin mesh in one fixed pose, given in the obj file. It is necessary to compute the Bind to Joint Transform once. This is stored with the joint.
Joints have fixed length. The joints form a rooted tree. The sliders allow one to control the rotation of one node's joint space to its parent's. 

<b>Simulation: </b> Physical Simulation of some systems with numerical integration

Input: (optional)
Command line arguments: Simulation.exe arg1 arg2. If arg1=e, the program uses forward Euler integration. If arg1=t, the program uses Trapezoidal integration. Otherwise, the program uses RK4 integration. arg2 is optional and is sthe time step as a float. It is .04f by default. 

While in the GUI, we use a similar camera. Any click shows the axes. Left+drag, right+drag are the same. Both+drag translates the objects but not the axes. The space bar resets the rotation. See the console for other typed commands while in the GUI. They include toggling the simulation and setting some parameters. Warning!! Clicking on the GUI and console a lot sometimes makes the display disappear until the program is terminated, and I can't reproduce it reliably. I would recommend clicking on the Title bar to select the GUI and not rotating much.

Design Notes: The main classes are ParticleSystem and a TimeStepper, both virtual. All particle systems extend ParticleSystem, and all time steppers (integration methods) extend TimeStepper. ParticleSystem has a state vector which would include position and velocity vectors for particle systems, but could include other parameters like time or temperature in more complicated systems. ParticleSystem has the method evalF(vector<Vector3f> state)=0, which gives the time derivative (i.e. evolution) of state, NOT the object that calls it. TimeStepper has the method takeStep(ParticleSystem* particleSystem,float stepSize)=0, which updates particleSystem's state to the time stepper's estimation of the state after a time step of stepSize. 

ClothSystem is implemented as a 2D array of particles with Hooke springs between adjacent particles, those (1,1) away, and those (2,0) away, with certain spring constant, mass, gravity, drag, and size parameters. PendulumSystem is an array of particles with hooke springs between adjacent particles with similar parameters. So it's not really a pendulum. SimpleSystem is... simple.

There is currently no collision detection. Changing the parameters may result in metastable states (i.e. one object is balanced on a spring above another when it shouldn't).

I did not place the following as different projects in the same solution because I couldn't figure out how to manage the differences

<b>Ray Tracing</b>: Ray Tracing to render scene descriptions into bitmaps.

Input: Command line arguments: I'll describe the arguments with spaces in between. A string in brackets denotes one argument, and the string is the description. Strings not in brackets should be exact. The last 5 are optional, and there are default values. Technically, some phrases can be moved around, and one can insert some garbage arguments for fun.

[program\_name] -input [input\_filename] -output [output\_filename] -size [width] [height] -bounces [number\_of\_bounces]

output: Renders the scene descriped in input\_filename, which should be a .txt type file, into output\_filename, which should be a .bmp, and the size is as given. The last three parameters should be positive ints. The grammar of the input should be reasonably understandable from the examples. The examples are scene01\_plane.txt etc.

Design Notes: The main way that this is simpler than the next project is that here, secondary rays are perfect reflections or 'refractions,' and there is no indirect light. Also, the light sources are simple. (Point Light or Directional).

Here are the main design ideas: Colors, Radiances, and related quantities are stored as Vector3fs corresponding to RGB ([0,1]^3). For each pixel, we create a corresponding ray based on the camera. We intersect it with the scene, keeping the closest intersection. We then find the color of that ray, which is based on many factors (see next paragraph). Actually, we create a 3 height x 3 width grid, imagine that it is in space in front of our perspective camera, sample each box at a random point (aka jitters), convolve with a Gaussian-like kernel, and average vectors in a 3x3 grid to get the height x width grid, which is the answer.

Here are the factors which determine the color of a ray: The material at that point, the lights which have an unblocked path to that point, and the reflected and transmitted rays at that point. These secondary rays (if they exist) are calculated as if the material is a perfect reflector/transmitter. The radiance of the first ray is basically a sum of the radiances from the lights (depends on the material/normal) and those of the secondary rays (uses recursion). The depth is limited by the bounces parameter. Note: If the ray doesn't hit anything, we use a background color, possibly a cube map.

Some classes: Ray: direction (normalized) and origin. Light: a certain method gives the direction to light, distance to light, and 'color', which is like radiance. Material: an instance stores stuff necessary for calculating radiance of lights according to the Phong model: Diffuse Color, Specular Color, texture or noise to have more elaborate diffuse colors, refraction index, shininess. Hit: normal, 't' value, and texture coordinates; always paired with a Ray.

Object3D: pointer to a material (so the object's material is uniform; this forces us to store information in other classes, such as normal/texture coordiantes in Hit). Also, an intersect method, which takes a const Ray&, Hit&, and float tmin. Updates Hit& to the nearest intersection of the object and the ray (stuff below tmin is ignored). The hit's 't' value is updated to the distance along the ray, and normals & texture coordinates are calculated similarly. The idea is that the radiance of lights to that intersection can be calculated using the Light, Material, and Hit classes. Material only is not possible because one doesn't know the normal, etc. Also, there is an any_intersect method for shadow rays.

Group: An Object3D which is really a contained for more basic Object3Ds. It's intended to be used for the entire group of objects in the scene

Scene Parser: parses the scene from the .txt file, makes the Group, Object3Ds with their materials, etc.

RayTracer: stores a group. Its main method is to take a ray and a number of bounces and do the above calculation, returning as a Vector3f. It computes the secondary rays / whether they exist and recursively calls itself. Observe that the recursion may branch, so the number of bounces should be small.

<b>Global Illumination</b>: One part is Monte Carlo Ray Tracing with luminous materials and a few refinements over the previous project for Global illumination. The second part is Photon Mapping with nearest neighbors to render chromatic dispersion with objects that interact with different wavelengths of light differently. Very basic now. Everything here is new except the part shared with Ray Tracing. 

This program renders using either Monte Carlo Ray Tracing or Photon Mapping using the input. (Changing this mode itself isn't meaningful; one must update the whole scene description. Also, Photon Mapping is not very good now.) They are in the same project because they use the same material class and object types.

Input: Command line arguments: Many fields are optional, and there are default values.

[file\_name] -input [input\_file\_name] -output [output\_file\_name] -size [width] [height] -bounces [max#bounces] -samples [#samples] -neighbors [#neighbors] -photonMultiplier [Photon\_Multiplier] -PhotonMapping

There are two main modes of the program: GI (Global Illumination / Monte Carlo Ray Tracing) and PM (Photon Mapping). If -PhotonMapping is one of the args, the mode is PM, and GI otherwise. #samples is # samples per supersample pixel. max#bounces is max # bounes for GI or PM. -photonMultiplier is a multiplier for all light sources, if photonMultiplier times sum(photon Ratio) ~ 2pi, and the whole scene is lit, and most of the photons end up on visible surfaces without too much reflection, then the picture should be reasonable. # neighbors is used for nearest neighbor searching. For photon mapping, the actual output has size 3 height x 3 width because that part of the program is slow and basic so far. Both parts are pretty slow.

Design Notes (GI): The main refinements of GI (Global Illumination) from the above project are that secondary rays can be in any direction, with importance sampling and there are luminous materials (which gives rise to area lights, soft shadows, and caustics). The main work of GI is in the SampleRay class. The main work of PM (Photon Mapping) is in the SamplePhoton class. These are not hte best names, but think: photons are emitted by light sources; rays are what the eye sees (I know this isn't standard lexicon). 

For GI, the colors = Vector3f as before. Instead of a RayTracer Class, we have a SampleRay class, which stores a group and has a recursive traceRay method (buying better names) It still finds the nearest intersection, but the secondary rays correspond to a continuous distribution. Thus, we use Monte Carlo integration and importance sampling. These are reflected in the different random direction methods. We still have the basic lights from before, but we also have diffusely luminous materials. A material's luminosity is added somewhere in the traceRay method (regardless of direction). The recursion is not branched; instead, we have an uneven distribution and uneven weights which results in an unbiased algorithm (I've seen this referred to as Monte Carlo Roulette). 

We still have the supersample grid, jitters, convolution, and down-sampling, but because of the noise of MC integration, we have many samples per supersample pixel. To reduce noise, the distribution of secondary rays is influenced by the index of the ray. However, this may create aliasing for small numbers of samples. 

Design Notes (PM): maybe being too ambitious. I've ignored the Light class here. Instead, I have Light Sources, which emit photons, or Ray Objects augmented with a wavelength field. The wavelength is 39-70 (int), in 10nm. LightSource has the Ray emitPhoton() method, which is called in proportion to the LightSource's photonRatio times the photonMultiplier

The first phase is Photon Mapping. For each Ray emitted, it is intersected with the group in the SamplePhoton class. It then interacts with the material according to the Phong model: diffuse, specular, or transmitted. currently, specular is perfect reflection for debugging purposes. The interaction also depends on the wavelength and the spectral properties of the material: Materials store information that determine the diffuse reflection ratio, the absorption ratio, and refraction index in terms of the wavelength. We store spatial information of the photons in a vector of Departures (called Departures because of my sampling method, see later)

The second phase is gathering. For each pixel of the 3 height x 3 width grid, we send just one ray and intersect with the scene, yielding a position in space. We find the nearest 'Departures', which determines the color of that pixel. 

My sampling method: As we map a particular photon, we keep track of when a photon is not absorbed i.e. a Departure. The idea is that the Departure could have gone towards the camera instead. When we gather the photons, we find the nearest departures, add their radiances toward the camera, and multiply by some ratio associated with the proximity of the departures: closer = brighter. The radiance is calculated using weights which assume that the photon is not absorbed: These are slightly different weights than used for mapping! One of the factors is the associated spectral RGB value, which are hard-coded. (They can go over 1 because, well I don't have all the factors calibrated). I can show that similar sampling techniques are unbiased on discrete examples, but I'm still figuring out the continuous case. I am pretty sure this is unbiased.

The nearest neighbors uses a 3d tree with partitioning according to which of the x range, y range, or z range is highest. 

Unfortunately, the nearest neighbors is really slow. I realize that one could just cast rays from each departure to the camera and increment the corresponding color value of the pixel. I have not done so for a few reasons (which are not particularly compelling, but oh well): This could be okay for more complex scenes, there could be less noise once I have a good interpolation method, and I wanted to demonstrate that I could write such a structure.

Some final notes: I drew a rainbow! On that note, observe that the color boundaries can now be much different than object boundaries, so I really need to study my interpolation to speed up this algorithm!


