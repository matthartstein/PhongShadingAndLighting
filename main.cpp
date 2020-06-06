//---------------------------------------
// Program: main.cpp
// Purpose: PC Graphics: Project #4
// Author:  Matthew Hartstein (010567629)
// Date:    Spring 2020
//---------------------------------------
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
using namespace std;
#ifdef MAC
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// Transformation variables
#define ROTATE 1
#define TRANSLATE 2
int xangle = 0;
int yangle = 0;
int zangle = 0;
int xpos = 0;
int ypos = 0;
int zpos = 0;
int mode = ROTATE;

// Surface variables
#define SIZE 500
float Px[SIZE][SIZE];       // X locations
float Py[SIZE][SIZE];       // Y locations
float Pz[SIZE][SIZE];       // Z locations

float Nx[SIZE][SIZE];       // X normals
float Ny[SIZE][SIZE];       // Y normals
float Nz[SIZE][SIZE];       // Z normals

float red[SIZE][SIZE];      // Red values
float green[SIZE][SIZE];    // Green values
float blue[SIZE][SIZE];     // Blue values
#define STEP 0.1

#include "shading.cpp"

//---------------------------------------
// Initialize surface points
//---------------------------------------
void init_surface(float Xmin, float Xmax, float Ymin, float Ymax) {

    // Open the depth file
    ifstream din;
    din.open("penny-depth.txt");
    int temp;

    // Check if file opened successfully, exit if not
    if(din.is_open()) {
        cout << "Extracting Depth Values. . ." << endl;

        // Initialize (x,y,z) coordinates of surface
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                Px[i][j] = Xmin + i * (Xmax - Xmin) / SIZE;
                Py[i][j] = Ymin + j * (Ymax - Ymin) / SIZE;
                din >> temp;
                Pz[i][j] = temp / 255.0;    // Update and scale depth values
            }
        }
    }
    else {
        cout << "ERROR: Could Not Open Depth File! Terminating. . ." << endl;
        exit(1);
    }

    // Output depth contents to log file
    ofstream dout;
    dout.open("depth_output.txt");
    for(int a = 0; a < SIZE; a++) {
        for(int b = 0; b < SIZE; b++) {
            dout << "X = " << Px[a][b] << " | " << "Y = " << Py[a][b]
            << " | " << "Z = " << Pz[a][b] << endl;
        }
    }

    // Close all depth objects
    dout.close();
    din.close();

    cout << "Success!" << endl << endl;

    // Open the image file
    ifstream input;
    input.open("penny-image.txt");

    // Check if file opened successfully, exit if not
    if(input.is_open()) {
        cout << "Extracting Image Values. . ." << endl;

        // Read and save image file contents
        int r; int g; int b;
        for(int m = 0; m < SIZE; m++) {
            for(int n = 0; n < SIZE; n++) {
                input >> r >> g >> b;
                red[m][n] = r;
                green[m][n] = g;
                blue[m][n] = b;
            }
        }
    }
    else {
        cout << "ERROR: Could Not Open Image File! Terminating. . ." << endl;
        exit(1);
    }

    // Output image contents to log file
    ofstream imgOut;
    imgOut.open("image_output.txt");
    for(int c = 0; c < SIZE; c++) {
        for(int d = 0; d < SIZE; d++) {
            imgOut << "Red = " << red[c][d] << endl;
            imgOut << "Green = " << green[c][d] << endl;
            imgOut << "Blue = " << blue[c][d] << endl << endl;
        }
    }

    // Close all image objects
    imgOut.close();
    input.close();

    cout << "Success!" << endl << endl;
    cout << "Log Files Successfully Created in Project's Root Directory." << endl << endl;
    cout << "--------------------------------------------------------------------" << endl << endl;
}

//---------------------------------------
// Initialize surface normals
//---------------------------------------
void init_normals() {

    // Initialize surface normals
    for (int i=0; i<SIZE-1; i++)
        for (int j=0; j<SIZE-1; j++) {

            // Get tangents S and T
            float Sx = Px[i+1][j] - Px[i][j];
            float Sy = Py[i+1][j] - Py[i][j];
            float Sz = Pz[i+1][j] - Pz[i][j];
            float Tx = Px[i][j+1] - Px[i][j];
            float Ty = Py[i][j+1] - Py[i][j];
            float Tz = Pz[i][j+1] - Pz[i][j];

            // Calculate N = S cross T
            Nx[i][j] = Sy*Tz - Sz*Ty;
            Ny[i][j] = Sz*Tx - Sx*Tz;
            Nz[i][j] = Sx*Ty - Sy*Tx;

            // Make unit vector
            float length = sqrt(Nx[i][j]*Nx[i][j]
                         + Ny[i][j]*Ny[i][j]
                         + Nz[i][j]*Nz[i][j]);
            if (length > 0) {
                Nx[i][j] /= length;
                Ny[i][j] /= length;
                Nz[i][j] /= length;
            }
        }

    // Handle last row and column
    for (int i=0; i<SIZE; i++) {
        Nx[i][SIZE-1] = Nx[i][SIZE-2];
        Ny[i][SIZE-1] = Ny[i][SIZE-2];
        Nz[i][SIZE-1] = Nz[i][SIZE-2];
        Nx[SIZE-1][i] = Nx[SIZE-2][i];
        Ny[SIZE-1][i] = Ny[SIZE-2][i];
        Nz[SIZE-1][i] = Nz[SIZE-2][i];
    }
}

//---------------------------------------
// Init function for OpenGL
//---------------------------------------
void init() {

    // Initialize OpenGL
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float radius = 1.2;
    glOrtho(-radius, radius, -radius, radius, -radius, radius);
    glEnable(GL_DEPTH_TEST);

    // Initialize smooth shading
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);
    init_light(GL_LIGHT0, 0, 1, 1, 0.5, 0.5, 0.5);
    init_light(GL_LIGHT1, 0, 0, 1, 0.5, 0.5, 0.5);
    init_light(GL_LIGHT2, 0, 1, 0, 0.5, 0.5, 0.5);

    // Initialize surface
    init_surface(-1.0, 1.0, -1.0, 1.0);
    init_normals();
}

//---------------------------------------
// Display callback for OpenGL
//---------------------------------------
void display() {

    // Incrementally rotate objects
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(xpos / 500.0, ypos / 500.0, zpos / 500.0);
    glRotatef(xangle, 1.0, 0.0, 0.0);
    glRotatef(yangle, 0.0, 1.0, 0.0);
    glRotatef(zangle, 0.0, 0.0, 1.0);

    // Initialize material properties
    init_material(Ka, Kd, Ks, 100 * Kp, 0.8, 0.6, 0.4);

    // Draw the surface
    for (int i = 0; i < SIZE-1; i++) {
        for (int j = 0; j < SIZE-1; j++) {
            glBegin(GL_POLYGON);
                glNormal3f(Nx[i][j], Ny[i][j], Nz[i][j]);
                glVertex3f(Px[i][j], Py[i][j], Pz[i][j]);
                glNormal3f(Nx[i + 1][j], Ny[i + 1][j], Nz[i + 1][j]);
                glVertex3f(Px[i + 1][j], Py[i + 1][j], Pz[i + 1][j]);
                glNormal3f(Nx[i + 1][j + 1], Ny[i + 1][j + 1], Nz[i + 1][j + 1]);
                glVertex3f(Px[i + 1][j + 1], Py[i + 1][j + 1], Pz[i + 1][j + 1]);
                glNormal3f(Nx[i][j + 1], Ny[i][j + 1], Nz[i][j + 1]);
                glVertex3f(Px[i][j + 1], Py[i][j + 1], Pz[i][j + 1]);
            glEnd();
        }
    }
    glFlush();
}

//---------------------------------------
// Keyboard callback for OpenGL
//---------------------------------------
void keyboard(unsigned char key, int x, int y) {

    // Initialize random surface
    if (key == 'i') {
        init_surface(-1.0, 1.0, -1.0, 1.0);
        init_normals();
    }

    // Determine if we are in ROTATE or TRANSLATE mode
    if ((key == 'r') || (key == 'R')) {
        printf("Type x y z to decrease or X Y Z to increase ROTATION angles.\n");
        mode = ROTATE;
    }
    else if ((key == 't') || (key == 'T')) {
        printf("Type x y z to decrease or X Y Z to increase TRANSLATION distance.\n");
        mode = TRANSLATE;
    }

    // Handle ROTATE
    if (mode == ROTATE) {
        if (key == 'x')
            xangle -= 5;
        else if (key == 'y')
            yangle -= 5;
        else if (key == 'z')
            zangle -= 5;
        else if (key == 'X')
            xangle += 5;
        else if (key == 'Y')
            yangle += 5;
        else if (key == 'Z')
            zangle += 5;
    }

    // Handle TRANSLATE
    if (mode == TRANSLATE) {
        if (key == 'x')
            xpos -= 5;
        else if (key == 'y')
            ypos -= 5;
        else if (key == 'z')
            zpos -= 5;
        else if (key == 'X')
            xpos += 5;
        else if (key == 'Y')
            ypos += 5;
        else if (key == 'Z')
            zpos += 5;
    }

    // Handle material properties
    if (key == 'a')
        Ka -= STEP;
    if (key == 'd')
        Kd -= STEP;
    if (key == 's')
        Ks -= STEP;
    if (key == 'p')
        Kp -= STEP;
    if (key == 'A')
        Ka += STEP;
    if (key == 'D')
        Kd += STEP;
    if (key == 'S')
        Ks += STEP;
    if (key == 'P')
        Kp += STEP;
    if (Ka < 0)
        Ka = 0;
    if (Kd < 0)
        Kd = 0;
    if (Ks < 0)
        Ks = 0;
    if (Kp < STEP)
        Kp = STEP;

    glutPostRedisplay();
}

//---------------------------------------
// Mouse callback for OpenGL
//---------------------------------------
void mouse(int button, int state, int x, int y) {

    // Handle mouse down
    static int xdown, ydown;
    if (state == GLUT_DOWN) {
        xdown = x;
        ydown = y;
    }

    // Handle ROTATE
    if ((mode == ROTATE) && (state == GLUT_UP)) {
        xangle += (y - ydown);
        yangle -= (x - xdown);
        zangle = 0;
        glutPostRedisplay();
    }

    // Handle TRANSLATE
    if ((mode == TRANSLATE) && (state == GLUT_UP)) {
        xpos += (x - xdown);
        ypos -= (y - ydown);
        glutPostRedisplay();
    }
}

//---------------------------------------
// Main program
//---------------------------------------
int main(int argc, char *argv[]) {

    // Create OpenGL window
    glutInit(&argc, argv);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(250, 250);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE | GLUT_DEPTH);
    glutCreateWindow("PC Graphics: Project #4");
    init();
    cout << "MAIN MENU:" << endl;
    printf("Type 'r' to enter ROTATE mode or 't' to enter TRANSLATE mode.\n");
    printf("You may also enter: 'a', 'A', 'd', 'D', 's', 'S', 'p', or 'P' to adjust material properties.\n");
    printf("Notes: Must click on OpenGL Window for keyboard/mouse commands to respond.\n\n");

    // Specify callback functions
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMainLoop();
    return 0;
}
