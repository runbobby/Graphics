#include "GL/freeglut.h"
#include <cmath>
#include <iostream>
#include <sstream>
#include <vector>
#include "vecmath.h"
#include <fstream>
using namespace std;

// Globals
int color_counter = 0;
GLdouble camera_x = 0;
GLdouble camera_y = 0;

// This is the list of points (3D vectors)
vector<Vector3f> vecv;

// This is the list of normals (also 3D vectors)
vector<Vector3f> vecn;

// This is the list of faces (indices into vecv and vecn)
vector<vector<unsigned> > vecf;


// You will need more global variables to implement color and position changes


// These are convenience functions which allow us to call OpenGL 
// methods on Vec3d objects
inline void glVertex(const Vector3f &a) 
{ glVertex3fv(a); }

inline void glNormal(const Vector3f &a) 
{ glNormal3fv(a); }


// This function is called whenever a "Normal" key press is received.
void keyboardFunc( unsigned char key, int x, int y )
{
    switch ( key )
    {
    case 27: // Escape key
        exit(0);
        break;
    case 'c':
        // add code to change color here
		// cout << "Unhandled key press " << key << "." << endl; 
		color_counter++;
        break;
    default:
        cout << "Unhandled key press " << key << "." << endl;        
    }

	// this will refresh the screen so that the user sees the color change
    glutPostRedisplay();
}

// This function is called whenever a "Special" key press is received.
// Right now, it's handling the arrow keys.
void specialFunc( int key, int x, int y )
{
    switch ( key )
    {
    case GLUT_KEY_UP: // oops I did camera
        // add code to change light position
		// cout << "Unhandled key press: up arrow." << endl;
		camera_y += 0.5;
		break;
    case GLUT_KEY_DOWN:
        // add code to change light position
		//cout << "Unhandled key press: down arrow." << endl;
		camera_y -= 0.5;
		break;
    case GLUT_KEY_LEFT:
        // add code to change light position
		//cout << "Unhandled key press: left arrow." << endl;
		camera_x -= 0.5;
		break;
    case GLUT_KEY_RIGHT:
        // add code to change light position
		//cout << "Unhandled key press: right arrow." << endl;
		camera_x += 0.5;
		break;
    }

	// this will refresh the screen so that the user sees the light position
    glutPostRedisplay();
}

// This function is responsible for displaying the object.
void drawScene(void)
{
    int i;

    // Clear the rendering window
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Rotate the image
    glMatrixMode( GL_MODELVIEW );  // Current matrix affects objects positions
    glLoadIdentity();              // Initialize to the identity

    // Position the camera at [0,0,5], looking at [0,0,0],
    // with [0,1,0] as the up direction.
    gluLookAt(camera_x, camera_y, 5.0,
              0.0, 0.0, 0.0,
              0.0, 1.0, 0.0);

    // Set material properties of object

	// Here are some colors you might use - feel free to add more
    GLfloat diffColors[4][4] = { {0.5, 0.5, 0.9, 1.0},
                                 {0.9, 0.5, 0.5, 1.0},
                                 {0.5, 0.9, 0.3, 1.0},
                                 {0.3, 0.8, 0.9, 1.0} };
    
	// Here we use the first color entry as the diffuse color
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, diffColors[(color_counter%4)]);

	// Define specular color and shininess
    GLfloat specColor[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat shininess[] = {100.0};

	// Note that the specular color and shininess can stay constant
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specColor);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
  
    // Set light properties

    // Light color (RGBA)
    GLfloat Lt0diff[] = {1.0,1.0,1.0,1.0};
    // Light position
	GLfloat Lt0pos[] = {1.0f, 1.0f, 5.0f, 1.0f};

    glLightfv(GL_LIGHT0, GL_DIFFUSE, Lt0diff);
    glLightfv(GL_LIGHT0, GL_POSITION, Lt0pos);

	// This GLUT method draws a teapot.  You should replace
	// it with code which draws the object you loaded.
	// glutSolidTeapot(1.0);
	for (unsigned int i = 0; i < vecf.size(); i++) {
		glBegin(GL_TRIANGLES);
		glNormal3d(vecn[vecf[i][1] - 1][0], vecn[vecf[i][1] - 1][1], vecn[vecf[i][1] - 1][2]);
		glVertex3d(vecv[vecf[i][0] - 1][0], vecv[vecf[i][0] - 1][1], vecv[vecf[i][0] - 1][2]);
		glNormal3d(vecn[vecf[i][3] - 1][0], vecn[vecf[i][3] - 1][1], vecn[vecf[i][3] - 1][2]);
		glVertex3d(vecv[vecf[i][2] - 1][0], vecv[vecf[i][2] - 1][1], vecv[vecf[i][2] - 1][2]);
		glNormal3d(vecn[vecf[i][5] - 1][0], vecn[vecf[i][5] - 1][1], vecn[vecf[i][5] - 1][2]);
		glVertex3d(vecv[vecf[i][4] - 1][0], vecv[vecf[i][4] - 1][1], vecv[vecf[i][4] - 1][2]);
		glEnd();
	}
    // Dump the image to the screen.
    glutSwapBuffers();


}

// Initialize OpenGL's rendering modes
void initRendering()
{
    glEnable(GL_DEPTH_TEST);   // Depth testing must be turned on
    glEnable(GL_LIGHTING);     // Enable lighting calculations
    glEnable(GL_LIGHT0);       // Turn on light #0.
}

// Called when the window is resized
// w, h - width and height of the window in pixels.
void reshapeFunc(int w, int h)
{
    // Always use the largest square viewport possible
    if (w > h) {
        glViewport((w - h) / 2, 0, h, h);
    } else {
        glViewport(0, (h - w) / 2, w, w);
    }

    // Set up a perspective view, with square aspect ratio
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // 50 degree fov, uniform aspect ratio, near = 1, far = 100
    gluPerspective(50.0, 1.0, 1.0, 100.0);
}

void loadInput()
{// load the OBJ file here
	/*cout << "Please enter input file name or \"0\" to skip: \n";
	//string iname;
	while (!cin);
	{
		cin >> iname;
	}
	if (iname == "0") return;/**/
	string iname = "torus.obj";
	ifstream ist{ iname };
	if (!ist) { cerr << "can't open input file."; return; }
	char buffer[128];
	while (ist.getline(buffer, 128)) { // max buffer size?
		stringstream ss{ buffer };
		string token;
		ss >> token;
		if(token=="v"){
			Vector3f v;
			ss >> v[0] >> v[1] >> v[2];
			vecv.push_back(v);
		}
		if (token == "vn") {
			Vector3f v;
			ss >> v[0] >> v[1] >> v[2];
			vecn.push_back(v);
		}
		if(token == "f"){
			unsigned int a, b, c, d, e, f, g, h, i; char j; // j should read / many times; not tested
			ss >> a >> j >> b >> j >> c >> d >> j >> e >> j >> f >> g >> j >> h >> j >> i;
			vecf.push_back({ a,c,d,f,g,i });
		}
	} 
}

// Main routine.
// Set up OpenGL, define the callbacks and start the main loop
int main( int argc, char** argv )
{
    loadInput();

    glutInit(&argc,argv);

    // We're going to animate it, so double buffer 
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );

    // Initial parameters for window position and size
    glutInitWindowPosition( 60, 60 );
    glutInitWindowSize( 360, 360 );
    glutCreateWindow("Assignment 0");

    // Initialize OpenGL parameters.
    initRendering();

    // Set up callback functions for key presses
    glutKeyboardFunc(keyboardFunc); // Handles "normal" ascii symbols
    glutSpecialFunc(specialFunc);   // Handles "special" keyboard keys

     // Set up the callback function for resizing windows
    glutReshapeFunc( reshapeFunc );

    // Call this whenever window needs redrawing
    glutDisplayFunc( drawScene );

    // Start the main loop.  glutMainLoop never returns.
    glutMainLoop( );

    return 0;	// This line is never reached.
}
