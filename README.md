# Graphics

Bobby Shen (Robert)

Here are some graphics projects I have worked on, written in MSVC++ 2010 or 2015. Currently, this majority of this repository is the assignments from the 6.837 course on MIT's OCW (from Fall 2012). The 'starter code' for these assignments consists of many structural elements or algorithms that deal with hardware, such as the structure of an OpenGL program, an arcball camera, some parsers, some class headers, some complete vector and matrix classes, object/scene descriptions in text, an acceleration structure, and so on. See http://ocw.mit.edu/courses/electrical-engineering-and-computer-science/6-837-computer-graphics-fall-2012/assignments/ (VC++ 2010, specifically). I have completed the rest myself. I decided not to upload assignments 0 and 1.

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

Input: Commant line arguments: 
