# Graphics

Bobby Shen (Robert)

Here are some graphics projects I have worked on, written in MSVC++ 2010 or 2015. Currently, this majority of this repository is the assignments from the 6.837 course on MIT's OCW (from Fall 2012). The 'starter code' for these assignments consists of many structural elements or algorithms that deal with hardware, such as the structure of an OpenGL program, an arcball camera, some parsers, some class headers, some complete vector and matrix classes, an acceleration structure, and so on. See http://ocw.mit.edu/courses/electrical-engineering-and-computer-science/6-837-computer-graphics-fall-2012/assignments/ (VC++ 2010, specifically). I have completed the rest myself. I decided not to upload assignments 0 and 1.

In general, my comments use ///, and the comments from the starter code use //. Some projects may require one to disable incremental linking.

Some specifics of each project:

SSD: Hierarchical modeling and Skeletal Subspace Deformation. I believe that this is incompatible with VC++ 2015. 
Input: Command line arguments: a2.exe


Use the sliders to define the Euler Angles to update each model. The z rotation is applied first, then y, then x. 

While in the OpenGL window, use the mouse to move the arcball camera: any click to show axes, L Click + drag to rotate, R Click + drag to dilate about the origin, Both Click + drag to translate the objects. Use space to reset the rotation but not the scale. 
