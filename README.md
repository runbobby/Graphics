# Graphics

Bobby Shen (Robert)

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

While in the GUI, we use a similar camera. Any click shows the axes. Left+drag, right+drag are the same. Both+drag translates the objects but not the axes. The space bar resets the rotation. See the console for other typed commands while in the GUI. They include toggling the simulation and setting some parameters. Warning!! Clicking on the GUI and console a lot sometimes makes the display disappear, and I can't reproduce it reliably. I would recommend clicking on the Title bar to select the GUI and not rotating much.

Design Notes: The main classes are ParticleSystem and a TimeStepper, both virtual. All particle systems extend ParticleSystem, and all time steppers (integration methods) extend TimeStepper. ParticleSystem has a state vector which would include position and velocity vectors for particle systems, but could include other parameters like time or temperature in more complicated systems. ParticleSystem has the method evalF(vector<Vector3f> state)=0, which gives the time derivative (i.e. evolution) of state, NOT the object that calls it. TimeStepper has the method takeStep(ParticleSystem* particleSystem,float stepSize)=0, which updates particleSystem's state to the time stepper's estimation of the state after a time step of stepSize. 

ClothSystem is implemented as a 2D array of particles with Hooke springs between adjacent particles, those (1,1) away, and those (2,0) away, with certain spring constant, mass, gravity, drag, and size parameters. PendulumSystem is an array of particles with hooke springs between adjacent particles with similar parameters. So it's not really a pendulum. SimpleSystem is... simple.

There is currently no collision detection. Changing the parameters may result in metastable states (i.e. one object is balanced on a spring above another when it shouldn't).

I did not place the following as different projects in the same solution because I couldn't figure out how to manage the differences

<b>Ray Tracing</b>: Ray Tracing to render scene descriptions into bitmaps.

Input: Command line arguments: I'll describe the arguments with spaces in between. A string in brackets denotes one argument, and the string is the description. Strings not in brackets should be exact

[program\_name] -input [input\_filename] -output [output\_filename] -size [width] [height] -bounces [number\_of\_bounces]

output: Renders the scene descriped in input\_filename, which should be a .txt type file, into output\_filename, which should be a .bmp, and the size is as given. The last three parameters should be positive ints. The grammar of the input should be reasonably understandable from the examples. The examples are scene01\_plane.txt etc.

Design Notes: The main way that this is simpler than the next project is that here, secondary rays are perfect reflections or 'refractions,' and there is no indirect light. Also, the light sources are simple. The main work is in the RayTracer class.

<b>Global Illumination</b>: One part is Monte Carlo Ray Tracing with luminous materials and a few refinements over the previous project for Global illumination. The second part is Photon Mapping with nearest neighbors to render chromatic dispersion with objects that interact with different wavelengths of light differently. Very basic now. Everything here is new except the part shared with Ray Tracing. 

This program renders using either Monte Carlo Ray Tracing or Photon Mapping using the input. (Changing this mode itself isn't meaningful; one must update the whole scene description. Also, Photon Mapping is not very good now.) They are in the same project because they use the same material class and object types.

Input: Command line arguments:

Design Notes: The main refinements of GI (Global Illumination) from the above project are that secondary rays can be in any direction, with importance sampling and there are luminous materials (which gives rise to area lights, soft shadows, and caustics). The main work of GI is in the SampleRay class. The main work of PM (Photon Mapping) is in the SamplePhoton class. These are not hte best names, but think: photons are emitted by light sources; rays are what the eye sees (I know this isn't standard lexicon).


