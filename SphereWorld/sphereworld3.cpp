// SphereWorld.cpp
// OpenGL SuperBible
// Demonstrates an immersive 3D environment using actors
// and a camera. This version adds lights and material properties
// and shadows.
// Program by Richard S. Wright Jr.

#include "share/gltools.h"	// OpenGL toolkit
#include "share/math3d.h"
#include "share/glframe.h"   // Frame Class
#include <math.h>


#define NUM_SPHERES      50
GLFrame    spheres[NUM_SPHERES];
GLFrame    frameCamera;
GLFrame    frametorus;
float delta=0;

// Light and material Data
GLfloat fLightPos[4]   = { -100.0f, 100.0f, 50.0f, 1.0f };  // Point source
GLfloat fNoLight[] = { 0.0f, 0.0f, 0.0f, 0.0f };
GLfloat fLowLight[] = { 0.25f, 0.25f, 0.25f, 1.0f };
GLfloat fBrightLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };


GLfloat fLightPos2[4]   = { -200.0f, 200.0f, 50.0f, 1.0f };  // Point source
GLfloat fNoLight2[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat fLowLight2[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat fBrightLight2[] = { 1.0f, 1.0f, 1.0f, 1.0f };


M3DMatrix44f mShadowMatrix;

static int xtorso=0,xneck=0,leftup_arm=0,leftdown_arm=0,rightup_arm=0,rightdown_arm=0,
left_thigh=0 ,right_thigh=0,left_calf=0,right_calf=0,tmp=0;
//////////////////////////////////////////////////////////////////
void torso(void)
{
    glPushMatrix();
    glTranslatef(0,0.5,0);
    glScalef(0.5,0.6,0.5);
    glutSolidSphere(1.5,15,10);
    glPopMatrix();
}
void shoulder()
{
    glPushMatrix();
    glScalef(0.65,0.15,0.15);
    glutSolidSphere(1.5,20,10);
    glPopMatrix();
}
void head()
{
    glPushMatrix();
    glTranslatef(0,0.5,0);
    glutSolidSphere(0.5,20,10);
    glPopMatrix();
}
void hips()
{
    glPushMatrix();
    glScalef(0.8,0.5,0.35);
    glutSolidSphere(0.8,20,10);
    glPopMatrix();
}
void pyramid(){
    glBegin(GL_TRIANGLES);           // Begin drawing the pyramid with 4 triangles
    // Front
    glVertex3f( 0.0f, 1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    
    // Right
    glVertex3f(0.0f, 1.0f, 0.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    
    // Back
    glVertex3f(0.0f, 1.0f, 0.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    
    // Left
    glVertex3f( 0.0f, 1.0f, 0.0f);
    glVertex3f(-1.0f,-1.0f,-1.0f);
    glVertex3f(-1.0f,-1.0f, 1.0f);
    glEnd();   // Done drawing the pyramid
}



//////////////////////////////////////////////////////////////////
// This function does any needed initialization on the rendering
// context.


void SetupRC()
{
    int iSphere;
    
    // Calculate shadow matrix
    M3DVector3f vPoints[3] = {{ 0.0f, -0.4f, 0.0f },
        { 10.0f, -0.4f, 0.0f },
        { 5.0f, -0.4f, -5.0f }};
    
    // Grayish background
    glClearColor(fLowLight[0], fLowLight[1], fLowLight[2], fLowLight[3]);
    
    // Cull backs of polygons
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    
    // Setup light parameters
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, fNoLight);
    glLightfv(GL_LIGHT0, GL_AMBIENT, fLowLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, fBrightLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, fBrightLight);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    
    // Get the plane equation from three points on the ground
    M3DVector4f vPlaneEquation;
    m3dGetPlaneEquation(vPlaneEquation, vPoints[0], vPoints[1], vPoints[2]);
    
    // Calculate projection matrix to draw shadow on the ground
    m3dMakePlanarShadowMatrix(mShadowMatrix, vPlaneEquation, fLightPos);
    
    
    // Mostly use material tracking
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glMateriali(GL_FRONT, GL_SHININESS, 128);
    
    // Randomly place the sphere inhabitants
    for(iSphere = 0; iSphere < NUM_SPHERES; iSphere++)
    {
        // Pick a random location between -20 and 20 at .1 increments
        float x = ((float)((rand() % 400) - 200) * 0.1f);
        float z = (float)((rand() % 400) - 200) * 0.1f;
        spheres[iSphere].SetOrigin(x, 0.0f, z);
    }
    
    frametorus.SetOrigin(0.0f, 0.0f, -2.5);
    
}


///////////////////////////////////////////////////////////
// Draw the ground as a series of triangle strips
void DrawGround(void)
{
    GLfloat fExtent = 20.0f;
    GLfloat fStep = 1.0f;
    GLfloat y = -0.4f;
    GLint iStrip, iRun;
    
    for(iStrip = -fExtent; iStrip <= fExtent; iStrip += fStep)
    {
        glBegin(GL_TRIANGLE_STRIP);
        glNormal3f(0.0f, 1.0f, 0.0f);   // All Point up
        
        for(iRun = fExtent; iRun >= -fExtent; iRun -= fStep)
        {
            glVertex3f(iStrip, y, iRun);
            glVertex3f(iStrip + fStep, y, iRun);
        }
        glEnd();
    }
}

///////////////////////////////////////////////////////////////////////
// Draw random inhabitants and the rotating torus/sphere duo

void DrawRobot(void){
    static GLfloat zRot = 0.0f;         // Rotation angle for animation
    zRot += 0.5f;
    /////////////////////upper part
    glPushMatrix();
    glTranslatef(0,4,0);
    glRotatef(xtorso,1.0,0.0,0.0);
    /////////////
    torso(); //draw the torso
    
    glPushMatrix();
    glTranslatef(0,1.5,0);
    shoulder(); //draw the shoulder
    // gltDrawUnitAxes();
    glPushMatrix();
    glRotatef((GLfloat)xneck,1,0,0);
    head(); //draw the head
    glPopMatrix();
    /////////////////////////////right hand
    glPushMatrix();
    glTranslatef(0.0,-1.5,0);
    glPushMatrix(); //draw the joint
    glTranslatef(1.0,1.5,0);
    glScalef(0.5,0.5,0.5);
    //////
    
    glutSolidSphere(0.6,20,10);
    glPopMatrix();
    
    glTranslatef(1.1,1.5,0);
    glRotatef((GLfloat)rightup_arm,1,0,0);
    glTranslatef(0,-0.9,0);
    glScalef(0.4,0.8,0.4);
    ///////////
    glutSolidSphere(0.8,20,10); //draw the right upper arm
    
				glPushMatrix();
    glTranslatef(0,-1,0);
    glRotatef((GLfloat)rightdown_arm,1,0,0);
    glTranslatef(0,-0.6,0);
    glScalef(0.5,0.8,0.5);
    glutSolidSphere(1,20,10); //draw the right down arm
    
    glTranslatef(0,-1.25,0);
    glScalef(4,1.0,4);
    glutSolidSphere(0.25,20,10); //draw the right palm
    
				glPopMatrix();
    glPopMatrix();
    /////////////////////////////left hand
    glPushMatrix();
    glTranslatef(0.0,-1.5,0);
    glPushMatrix();//draw the joint
    glTranslatef(-1.0,1.5,0);
    glScalef(0.5,0.5,0.5);
    glutSolidSphere(0.6,20,10);
    glPopMatrix();
    
				glTranslatef(-1.1,1.5,0);
				glRotatef((GLfloat)leftup_arm,1,0,0);
    glTranslatef(0,-0.9,0);
    glScalef(0.4,0.8,0.5);
    glutSolidSphere(0.8,20,10); //draw the left upper arm
    
				glPushMatrix();
    glTranslatef(0,-1,0);
    glRotatef((GLfloat)leftdown_arm,1,0,0);
    glTranslatef(0,-0.6,0);
    glScalef(0.5,0.8,0.5);
    glutSolidSphere(1,20,10); //draw the left down arm
    
    glTranslatef(0,-1.25,0);
    glScalef(4,1.0,4);
    glutSolidSphere(0.25,20,10); //draw the left palm
				glPopMatrix();
    
    glPopMatrix();
    glPopMatrix();
    
    /////////////////////lower part
    glPushMatrix();
    glTranslatef(0.0,-0.8,0);
    hips(); //draw the hips
    
    glPushMatrix();
				glTranslatef(0.5,0,0);
				glRotatef((GLfloat)right_thigh,1,0,0);
				glTranslatef(0,-0.9,0);
				glScalef(0.3,0.65,0.3);
				glutSolidSphere(1,20,10); //draw the right thigh
    
				glPushMatrix();
    glTranslatef(0,-0.9,0);
    glRotatef((GLfloat)right_calf,1,0,0);
    glTranslatef(0,-1,0);
    glScalef(0.6,0.9,0.6);
    glutSolidSphere(1,20,10); //draw the right calf
    
    glTranslatef(0,-1.25,0);
    glScalef(4,1.0,4);
    glutSolidSphere(0.25,20,10); //draw the right foot
    glPopMatrix();
    glPopMatrix();
    
    
    glPushMatrix();
				glTranslatef(-0.5,0,0);
				glRotatef((GLfloat)left_thigh,1,0,0);
				glTranslatef(0,-0.9,0);
				glScalef(0.3,0.65,0.3);
				glutSolidSphere(1,20,10); //draw the left thigh
    
				glPushMatrix();
    glTranslatef(0,-0.9,0);
    glRotatef((GLfloat)left_calf,1,0,0);
    glTranslatef(0,-1,0);
    glScalef(0.6,0.9,0.6);
    glutSolidSphere(1,20,10); //draw the left calf
    
    glTranslatef(0,-1.25,0);
    glScalef(4,1.0,4);
    glutSolidSphere(0.25,20,10); //draw the right foot
    
				glPopMatrix();
    glPopMatrix();
    ///////////////////////////////////////////////////rleg
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();
}
//+ Draw random inhabitants and the rotating torus/sphere duo



void DrawInhabitants(GLint nShadow)
{
    static GLfloat yRot = 0.0f;         // Rotation angle for animation
    GLint i;
    
    if(nShadow == 0)
        yRot += 0.5f;
    else
        glColor3f(0.0f, 0.0f, 0.0f);
    
    // Draw the randomly located spheres
    if(nShadow == 0)
        glColor3f(0.0f, 1.0f, 0.0f);
    
    
    for(i = 0; i < NUM_SPHERES; i++)
    {
        glPushMatrix();
        spheres[i].ApplyActorTransform();
        glutSolidSphere(0.1f, 13, 26);
        glPopMatrix();
    }
    
    glPushMatrix();
    frametorus.ApplyActorTransform();
    //glTranslatef(0.0f, 0.0f, -2.5f);
    if(nShadow == 0)
        glColor3f(0.0f, 0.0f, 1.0f);
    
    glPushMatrix();
    //glRotatef(yRot, 0.0f, 1.0f, 0.0f);
    //gltDrawTorus(0.35, 0.15, 61, 37);
    DrawRobot();
    
    glMaterialfv(GL_FRONT, GL_SPECULAR, fNoLight);
    glPopMatrix();
}


// Called to draw scene
void RenderScene(void)
{
    // Clear the window with current clearing color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glPushMatrix();
    frameCamera.ApplyCameraTransform();
    
    // Position light before any other transformations
    glLightfv(GL_LIGHT0, GL_POSITION, fLightPos);
    
    // Draw the ground
    glColor3f(0.60f, .40f, .10f);
    DrawGround();
    
    // Draw shadows first
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glMultMatrixf(mShadowMatrix);
    DrawInhabitants(1);
    glPopMatrix();
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    
    // Draw inhabitants normally
    DrawInhabitants(0);
    
    glPopMatrix();
    
    // Do the buffer Swap
    glutSwapBuffers();
}

void bf(int i)
{
    /*
     Your Implementation
     */
    
    glutPostRedisplay();
}

void myKeys(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 'w':
            /* Your Implementation */
            //Ex:  frametorus.????
            break;
        case 's':
            /* Your Implementation */
            
            break;
        case 'a':
            /* Your Implementation */
            break;
        case 'd':
            /* Your Implementation */
            break;
            
        case 'b':
            bf(0);
            break;
    }
    glutPostRedisplay();
}


// Respond to arrow keys by moving the camera frame of reference
void SpecialKeys(int key, int x, int y)
{
    if(key == GLUT_KEY_UP)
        frameCamera.MoveForward(0.1f);
    
    if(key == GLUT_KEY_DOWN)
        frameCamera.MoveForward(-0.1f);
    
    if(key == GLUT_KEY_LEFT)
        frameCamera.RotateLocalY(0.1f);
    
    if(key == GLUT_KEY_RIGHT)
        frameCamera.RotateLocalY(-0.1f);
    
    // Refresh the Window
    glutPostRedisplay();
}



///////////////////////////////////////////////////////////
// Called by GLUT library when idle (window not being
// resized or moved)
void TimerFunction(int value)
{
    // Redraw the scene with new coordinates
    glutPostRedisplay();
    glutTimerFunc(3,TimerFunction, 1);
}

void ChangeSize(int w, int h)
{
    GLfloat fAspect;
    
    // Prevent a divide by zero, when window is too short
    // (you cant make a window of zero width).
    if(h == 0)
        h = 1;
    
    glViewport(0, 0, w, h);
    
    fAspect = (GLfloat)w / (GLfloat)h;
    
    // Reset the coordinate system before modifying
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    // Set the clipping volume
    gluPerspective(35.0f, fAspect, 1.0f, 50.0f);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800,600);
    glutCreateWindow("OpenGL SphereWorld Demo");
    glutReshapeFunc(ChangeSize);
    glutDisplayFunc(RenderScene);
    glutKeyboardFunc(myKeys);
    glutSpecialFunc(SpecialKeys);
    
    SetupRC();
    glutTimerFunc(33, TimerFunction, 1);
    
    glutMainLoop();
    
    return 0;
}