# Graphics

Bobby Shen (Robert)

Here are some graphics projects I have worked on, written in MSVC++ 2010 or 2015. Currently, this majority of this repository is the assignments from the 6.837 course on MIT's OCW (from Fall 2012). The 'starter code' for these assignments consists of many structural elements or algorithms that deal with hardware, such as the structure of an OpenGL program, an arcball camera, some parsers, some class headers, some complete vector and matrix classes, object/scene descriptions in text, an acceleration structure, and so on. See http://ocw.mit.edu/courses/electrical-engineering-and-computer-science/6-837-computer-graphics-fall-2012/assignments/ (VC++ 2010, specifically). I did not change the started code except to set (0,0,0) normalized equal to (0,0,1) in some places (See Vector3f::normalized()). I have completed the rest myself. I decided not to upload assignments 0 and 1.

In general, my comments use ///, and the comments from the starter code use //. Some projects may require one to disable incremental linking. Every project is in VC++ 2015 except SSD. To directly run an .exe, move it from the release/debug folder into the main folder.

The files a2soln.exe a3soln.exe a4soln.exe are sample solutions from the OCW (with no source code)

Some specifics of each project:

<b>SSD</b>: Hierarchical modeling and Skeletal Subspace Deformation. The FLTK parts seem to be incompatible with VC++ 2015.

Input: Command line arguments: SSD.exe data/ModelN (2 arguments), where N is the digit 1, 2, 3, or 4. This loads the corresponding SSD model. Use the sliders to define the Euler Angles to update each model. The z rotation is applied first, then y, then x. While in the OpenGL window, use the mouse to move the arcball camera: L Click + drag to rotate, R Click + drag to dilate about the origin, Both Click + drag to translate the objects and axes. Press s to toggle between drawing the skin and the skeleton.

Design notes: Uses SSD and triangles with flat normals so that there is a faceted appearance. Doesn't handle extreme rotations or collisions at all.

There are three main types of spaces: (Current) World Space, Joint Space, and Bind World Space. 

World Space: what the skeleton looks like in the world given the current state. 

Joint Space: A local coordinate system in which the joint is at the origin. Points on the skin which are exclusively attached to that joint have a constant location in joint space. For example, the forearm has a constant location in the joint space of the elbow. The Joint to World Transform is a function of the current state (of rotations) and is calculated recursively after each input update, using my own matrix stack. The transform from one node's joint space to its parent is a function of the length (fixed) and the input angles. Both are stored with the joint.

Bind Space: The location of the skin mesh in one fixed pose, given in the obj file. It is necessary to compute the Bind to Joint Transform once. This is stored with the joint.
Joints have fixed length. The joints form a rooted tree. The sliders allow one to control the rotation of one node's joint space to its parent's. 

<b>Simulation: </b> Physical Simulation of some systems with numerical integration

Input: Command line arguments: Simulation.exe arg1 arg2. If arg1=e, the program uses forward Euler integration. If arg1=t, the program uses Trapezoidal integration. Otherwise, the program uses RK4 integration. arg2 is optional and is sthe time step as a float. It is .04f by default. 

While in the GUI, we use a similar camera. Any click shows the axes. Left+drag, right+drag are the same. Both+drag translates the objects but not the axes. The space bar resets the rotation. See the console for other typed commands while in the GUI. They include toggling the simulation and setting some parameters. Warning!! Clicking on the GUI a lot sometimes makes the display disappear, and I can't reproduce it reliably. I would recommend clicking on the Title bar to select the GUI and not clicking to rotate much.

Design Notes: The main classes are ParticleSystem and a TimeStepper, both virtual. All particle systems extend ParticleSystem, and all time steppers (integration methods) extend TimeStepper. ParticleSystem has a state vector which would include position and velocity vectors for particle systems, but could include other parameters like time or temperature in more complicated systems. ParticleSystem has the method evalF(vector<Vector3f> state)=0, which gives the time derivative (i.e. evolution) of state, NOT the object that calls it. TimeStepper has the method takeStep(ParticleSystem* particleSystem,float stepSize)=0, which updates particleSystem's state to the time stepper's estimation of the state after a time step of stepSize. 

ClothSystem is implemented as a 2D array of particles with Hooke springs between adjacent particles, those (1,1) away, and those (2,0) away, with certain spring constant, mass, gravity, drag, and size parameters. PendulumSystem is an array of particles with hooke springs between adjacent particles with similar parameters. So it's not really a pendulum. SimpleSystem is... simple.

There is currently no collision detection.

<b>Ray Tracing</b>: Ray Casting and Ray Tracing to render scene descriptions into bitmaps.

Input: Command line arguments: 

Design Notes:

<b>Global Illumination</b>: One part is Ray Tracing with luminous materials for Global Illumination. The second part is Photon Mapping with nearest neighbors to render chromatic dispersion with objects that interact with different wavelengths of light differently. Very basic now. Everything here is new except the part shared with Ray Tracing. Still outputs bitmaps.

Input: Command line arguments:

Design Notes:
