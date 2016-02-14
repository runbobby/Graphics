#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>
#include <vector>
#include <random>

#ifdef _WIN32
#include "GL/freeglut.h"
#else
#include <GL/glut.h>
#endif
#include <vecmath.h>
#include "camera.h"


#include "TimeStepper.hpp"
#include "simpleSystem.h"
#include "pendulumSystem.h"
#include "ClothSystem.h" 

using namespace std;

// Globals here.
namespace
{

    ParticleSystem *system;
    TimeStepper * timeStepper;
	float step;
	int delay=20; // milliseconds
	int system_type;

  // initialize your particle systems
	void initSystem(int argc, char * argv[])
	{
		if (argc < 2)
		{
			//cout << "usage: filename integrator_type [time step = .04f] \n integrator_type is e (ForwardEuler), t (Trapezoidal), or r/anything else (RK4)";
			step = .04f; timeStepper = new RK4(); 
		}
		else
		{
			if (argv[1][0] == 'e') { timeStepper = new ForwardEuler(); /*cout<<"Euler";*/ }
			else {
				if (argv[1][0] == 't') { timeStepper = new Trapezoidal(); /*cout<<"Trap";*/ }
				else { timeStepper = new RK4(); /*cout<<"RK4";*/ }
			}
			if (argc == 2) { step = .04f; }
			else
			{
				stringstream ss(argv[2]);
				float f = 0.04f;
				ss >> f;
				step = __min(.000001f, abs(f)); ///avoid errors
			}
		}
		cout << "Program usage: while in the GUI, type t to cycle through the types of simulation. Enter field as needed in the console. s: Enter step size of simulation; i: Enter integration system; d: Enter delay (milliseconds between frames added to computation time)." << endl;

		// seed the random number generator with the current time
		srand(time(NULL));

		system_type = 0;
		system = new SimpleSystem;
	}

  // Take a step forward for the particle shower
	void stepSystem()
	{
		if (timeStepper != 0) {
			timeStepper->takeStep(system, step);
		}
	}

  // Draw the current particle positions
	void drawSystem()
	{

		// Base material colors (they don't change)
		GLfloat particleColor[] = { 0.4f, 0.7f, 1.0f, 1.0f };
		GLfloat floorColor[] = { 1.0f, 0.0f, 0.0f, 1.0f };

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, particleColor);

		glutSolidSphere(0.1f, 10.0f, 10.0f);

		system->draw();


		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, floorColor);
		glPushMatrix();
		glTranslatef(0.0f, -5.0f, 0.0f);
		glScaled(50.0f, 0.01f, 50.0f);
		glutSolidCube(1);
		glPopMatrix();

	}

        

    //-------------------------------------------------------------------
    
        
    // This is the camera
    Camera camera;

    // These are state variables for the UI
    bool g_mousePressed = false;

    // Declarations of functions whose implementations occur later.
    void arcballRotation(int endX, int endY);
    void keyboardFunc( unsigned char key, int x, int y);
    void specialFunc( int key, int x, int y );
    void mouseFunc(int button, int state, int x, int y);
    void motionFunc(int x, int y);
    void reshapeFunc(int w, int h);
    void drawScene(void);
    void initRendering();



    // This function is called whenever a "Normal" key press is
    // received.
	///Update certain methods such as m, p if one adds modes
	///I am not very good at cleaning up input elegantly
	///I should probably update the constexprs to an enum class if I get around to it
    void keyboardFunc( unsigned char key, int x, int y )
    {
		constexpr int numberOfModes = 3;
		constexpr int simpleMode = 0;
		constexpr int pendulumMode = 1;
		constexpr int clothMode = 2;
        switch ( key )
        {
        case 27: // Escape key
            exit(0);
            break;
        case ' ':
        {
            Matrix4f eye = Matrix4f::identity();
            camera.SetRotation( eye );
            camera.SetCenter( Vector3f::ZERO );
            break;
        }
		case 't':
		{
			system_type++; 
			delete system;
			if (system_type == numberOfModes) { system_type = 0; system = new SimpleSystem; }
			else {
				if (system_type == pendulumMode) { ///next is pendulum
					cout << "\nEnter number of particles in pendulum string: ";
					int n = 2;
					string a;
					std::getline(cin, a);
					stringstream ss(a);
					ss >> n;
					cout << endl;
					system = new PendulumSystem(__max(2, n)); ///avoid errors
					cout << "Press 'p' to set parameters. \n";
					//cin.clear();
					//cin.ignore(INT_MAX, '\n');
				}
				else { ///next is cloth
					cout << "Enter number of particles across and down in cloth system as two ints in one line:";
					int h = 30; int w = 30;
					string a;
					std::getline(cin, a);
					stringstream ss(a);
					ss >> w >> h;
					cout << "Press 'm' to teleport one corner of the cloth. I know this is not very elegant. Press 'p' to set parameters \n";
					system = new ClothSystem(__max(2,h), __max(2,w)); ///avoid errors
					//cin.clear();
					//cin.ignore(INT_MAX, '\n');
				}
			}
			break;
		}
		case 'm':
		{
			if (system_type = clothMode) { static_cast<ClothSystem*>(system)->incrementCorner(); }
			break;
		}
		case 's':
			{
				cout<<"Enter step size: ";
				float t;
				string a;
				std::getline(cin, a);
				stringstream ss(a);
				ss>>t;
				if (t <= 0) { cout << "Input is not positive. Ignored"; }
				else step=t;
				cout << endl;
				//cin.clear();
				//cin.ignore(INT_MAX, '\n');
				break;
			}
		case 'i':
			{
				cout<<"\nEnter integration type: e, t, or r: ";
				char c = 'r';
				string a;
				std::getline(cin, a);
				stringstream ss(a);
				ss>>c;
				delete timeStepper;
				if(c == 'e'){cout << "Now using Forward Euler integration. \n"; timeStepper = new ForwardEuler();}
				if(c == 't'){cout << "Now using Trapezoidal integration. \n"; timeStepper = new Trapezoidal();}
				if(c != 'e' && c != 't'){cout << "Now using Runge Kutta integration. \n"; timeStepper = new RK4();}
				//cin.clear();
				//cin.ignore(INT_MAX, '\n');
				break;
			}
		case 'd':
		{
			cout << "\nEnter delay in milliseconds (int) in addition to calculation speed. ";
			int t = 0;
			string a;
			std::getline(cin, a);
			stringstream ss(a);
			ss >> t;
			if (t <= 0) { cout << "Input is not a positive integer. Ignored. "; }
			else delay = t;
			cout << endl;
			//cin.clear();
			//cin.ignore(INT_MAX, '\n');
			break;
		}
		/*case 'r':
			{
				glutPostRedisplay();
				break;
			}/**/
		case 'p':
		{
			if (system_type == simpleMode) { cout << "No parameters to set.\n"; break; }
			if (system_type == clothMode) ///Cloth
			{
				cout << "\nEnter the following parameters as floats in order as one line. Enter 0 to leave that parameter unchanged: mass, gravity, drag, structurek, sheark, flexionk: ";
				///input
				float mass = 0, gravity = 0, drag = 0, structurek = 0, sheark = 0, flexionk = 0;
				char buffer[255];
				cin.getline(buffer, 255);
				stringstream ss(buffer);
				ss >> mass;
				ss >> gravity;
				ss >> drag;
				ss >> structurek;
				ss >> sheark;
				ss >> flexionk;
				static_cast<ClothSystem*>(system)->setParameters(mass, gravity, drag, structurek, sheark, flexionk);
				//cin.clear();
				//cin.ignore(INT_MAX, '\n');
				break;
			}
			if (system_type == pendulumMode) ///Pendulum
			{
				cout << "\nEnter the following parameters as floats in order. Enter 0 to leave that parameter unchanged: springk, rest_length, gravity, drag, mass: ";
				///input
				float springk = 0, rest_length = 0, gravity=0, drag = 0, mass = 0;
				char buffer[255];
				cin.getline(buffer, 255);
				stringstream ss(buffer);
				ss >> springk;
				ss >> rest_length;
				ss >> gravity;
				ss >> drag;
				ss >> mass;
				static_cast<PendulumSystem*>(system)->setParameters(springk, rest_length, gravity, drag, mass);
				//cin.clear();
				//cin.ignore(INT_MAX, '\n');
				break;
			}
		}
        default:
            cout << "Unhandled key press " << key << "." << endl;        
        }

        glutPostRedisplay();
    }

    // This function is called whenever a "Special" key press is
    // received.  Right now, it's handling the arrow keys.
    void specialFunc( int key, int x, int y )
    {
        switch ( key )
        {

        }
        //glutPostRedisplay();
    }

    //  Called when mouse button is pressed.
    void mouseFunc(int button, int state, int x, int y)
    {
        if (state == GLUT_DOWN)
        {
            g_mousePressed = true;
            
            switch (button)
            {
            case GLUT_LEFT_BUTTON:
                camera.MouseClick(Camera::LEFT, x, y);
                break;
            case GLUT_MIDDLE_BUTTON:
                camera.MouseClick(Camera::MIDDLE, x, y);
                break;
            case GLUT_RIGHT_BUTTON:
                camera.MouseClick(Camera::RIGHT, x,y);
            default:
                break;
            }                       
        }
        else
        {
            camera.MouseRelease(x,y);
            g_mousePressed = false;
        }
        glutPostRedisplay();
    }

    // Called when mouse is moved while button pressed.
    void motionFunc(int x, int y)
    {
        camera.MouseDrag(x,y);        
    
        glutPostRedisplay();
    }

    // Called when the window is resized
    // w, h - width and height of the window in pixels.
    void reshapeFunc(int w, int h)
    {
        camera.SetDimensions(w,h);

        camera.SetViewport(0,0,w,h);
        camera.ApplyViewport();

        // Set up a perspective view, with square aspect ratio
        glMatrixMode(GL_PROJECTION);

        camera.SetPerspective(50);
        glLoadMatrixf( camera.projectionMatrix() );
    }

    // Initialize OpenGL's rendering modes
    void initRendering()
    {
        glEnable(GL_DEPTH_TEST);   // Depth testing must be turned on
        glEnable(GL_LIGHTING);     // Enable lighting calculations
        glEnable(GL_LIGHT0);       // Turn on light #0.

        glEnable(GL_NORMALIZE);

        // Setup polygon drawing
        glShadeModel(GL_SMOOTH);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

        // Clear to black
        glClearColor(0,0,0,1);
    }

    // This function is responsible for displaying the object.
    void drawScene(void)
    {
        // Clear the rendering window
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode( GL_MODELVIEW );  
        glLoadIdentity();              

        // Light color (RGBA)
        GLfloat Lt0diff[] = {1.0,1.0,1.0,1.0};
        GLfloat Lt0pos[] = {3.0,3.0,5.0,1.0};
        glLightfv(GL_LIGHT0, GL_DIFFUSE, Lt0diff);
        glLightfv(GL_LIGHT0, GL_POSITION, Lt0pos);

        glLoadMatrixf( camera.viewMatrix() );

        // THIS IS WHERE THE DRAW CODE GOES.

        drawSystem();

        // This draws the coordinate axes when you're rotating, to
        // keep yourself oriented.
        if( g_mousePressed )
        {
            glPushMatrix();
            Vector3f eye = camera.GetCenter();
            glTranslatef( eye[0], eye[1], eye[2] );

            // Save current state of OpenGL
            glPushAttrib(GL_ALL_ATTRIB_BITS);

            // This is to draw the axes when the mouse button is down
            glDisable(GL_LIGHTING);
            glLineWidth(3);
            glPushMatrix();
            glScaled(5.0,5.0,5.0);
            glBegin(GL_LINES);
            glColor4f(1,0.5,0.5,1); glVertex3f(0,0,0); glVertex3f(1,0,0);
            glColor4f(0.5,1,0.5,1); glVertex3f(0,0,0); glVertex3f(0,1,0);
            glColor4f(0.5,0.5,1,1); glVertex3f(0,0,0); glVertex3f(0,0,1);

            glColor4f(0.5,0.5,0.5,1);
            glVertex3f(0,0,0); glVertex3f(-1,0,0);
            glVertex3f(0,0,0); glVertex3f(0,-1,0);
            glVertex3f(0,0,0); glVertex3f(0,0,-1);

            glEnd();
            glPopMatrix();

            glPopAttrib();
            glPopMatrix();
        }
                 
        // Dump the image to the screen.
        glutSwapBuffers();
    }

    void timerFunc(int t)
    {
        stepSystem();

        glutPostRedisplay();

        glutTimerFunc(delay, &timerFunc, delay);
    }

    

    
    
}

// Main routine.
// Set up OpenGL, define the callbacks and start the main loop
int main( int argc, char* argv[] )
{
    glutInit( &argc, argv );

    // We're going to animate it, so double buffer 
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );

    // Initial parameters for window position and size
    glutInitWindowPosition( 60, 60 );
    glutInitWindowSize( 600, 600 );
    
    camera.SetDimensions( 600, 600 );

    camera.SetDistance( 10 );
    camera.SetCenter( Vector3f::ZERO );
    
    glutCreateWindow("Assignment 4");

    // Initialize OpenGL parameters.
    initRendering();

    // Setup particle system
    initSystem(argc,argv);

    // Set up callback functions for key presses
    glutKeyboardFunc(keyboardFunc); // Handles "normal" ascii symbols
    glutSpecialFunc(specialFunc);   // Handles "special" keyboard keys

    // Set up callback functions for mouse
    glutMouseFunc(mouseFunc);
    glutMotionFunc(motionFunc);

    // Set up the callback function for resizing windows
    glutReshapeFunc( reshapeFunc );

    // Call this whenever window needs redrawing
    glutDisplayFunc( drawScene );

    // Trigger timerFunc every 20 msec
	delay = 20;
    glutTimerFunc(delay, timerFunc, delay);

        
    // Start the main loop.  glutMainLoop never returns.
    glutMainLoop();

    return 0;	// This line is never reached.
}
