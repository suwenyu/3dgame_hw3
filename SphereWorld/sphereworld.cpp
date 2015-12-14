// SphereWorld.cpp
// OpenGL SuperBible
// Demonstrates an immersive 3D environment using actors
// and a camera. This version adds lights and material properties
// and shadows.
// Program by Richard S. Wright Jr.

#include "gltools.h"	// OpenGL toolkit
#include "math3d.h"
#include "glframe.h"   // Frame Class
#include <math.h>


#define NUM_SPHERES      50
GLFrame    spheres[NUM_SPHERES];

#define NUM_SQUARE 1
GLFrame square[NUM_SQUARE];

GLFrame    frameCamera;
GLFrame    frametorus;
float delta=0;

//float light_z = 20.0;
// Light and material Data
GLfloat fLightPos[4]   = { -100.0, 100.0, 50.0, 1.0f };  // Point source
GLfloat fNoLight[] = { 0.0f, 0.0f, 0.0f, 0.0f };
GLfloat fLowLight[] = { 0.25f, 0.25f, 0.25f, 1.0f };
GLfloat fBrightLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };

GLfloat fLightPos1[4] = {100.0 , 50.0 , 50.0 ,1.0};
GLfloat fLowLight1[] = {0.3f , 0.3f , 0.3f , 0.3f };
GLfloat fBrightLight1[] = {0.7 , 0.7 , 0.7 , 0.7};

GLfloat material_Ka[] = {0.11, 0.06 , 0.11, 1.00};
GLfloat material_Kd[] = {0.43 , 0.47 ,0.54 , 1.00};
GLfloat material_Ks[] = {0.33 , 0.33 , 0.52 , 1.00};
GLfloat material_Ke[] = {0.00 , 0.00 , 0.00 , 0.00 };
GLfloat material_Se = 10;

//GLfloat light_change[3] = {3,3,-1};

static int spin  = 0;


int leftknee = 0 , rightknee = 0 , leftthigh= 0 , rightthigh = 0;
int leftarm = 0 , leftshoulder = 0 , rightarm = 0 , rightshoulder = 0;
int head = 0;
int x = 0 , y = 0 ;
int CubeAngle = 0 ;




M3DMatrix44f mShadowMatrix;



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
    
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, fNoLight);
    glLightfv(GL_LIGHT1 , GL_AMBIENT , fLowLight1);
    glLightfv(GL_LIGHT1 , GL_DIFFUSE , fLowLight1);
    glLightfv(GL_LIGHT1, GL_SPECULAR, fBrightLight1);
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT1);
    
    // Get the plane equation from three points on the ground
    M3DVector4f vPlaneEquation;
    m3dGetPlaneEquation(vPlaneEquation, vPoints[0], vPoints[1], vPoints[2]);
    
    // Calculate projection matrix to draw shadow on the ground
    m3dMakePlanarShadowMatrix(mShadowMatrix, vPlaneEquation, fLightPos);
    
    //m3dMakePlanarShadowMatrix(mShadowMatrix, vPlaneEquation, fLightPos1);
    
    
    
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
    
    frametorus.SetOrigin(0.0f, 1.2f, -7.0);
    
    
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
void DrawInhabitants(GLint nShadow)
{
    

    static GLfloat yRot = 0.0f;         // Rotation angle for animation
    GLint i;
    
    if(nShadow == 0)
        yRot += 0.5f;
    else
        glColor3f(0.0f, 0.0f,0.0f);
    
    // Draw the randomly located spheres
    if(nShadow == 0)
        glColor3f(0.0f, 1.0f, 0.0f);
    
    
    for(i = 0; i < NUM_SPHERES; i++)
    {
        glPushMatrix();
        spheres[i].ApplyActorTransform();
        glMaterialfv(GL_FRONT, GL_EMISSION, material_Ke);
        glutSolidSphere(0.1f, 13, 26);
        glPopMatrix();
    }
    
    if (nShadow == 0) {
        glColor3f(1, 0, 1);
    }
    
    for(i = 0 ; i < NUM_SQUARE ; i++)
    {
        glPushMatrix();
        glRotated(CubeAngle, 1.0, 1.0, 0.0);
        square[i].ApplyActorTransform();
        glMaterialfv(GL_FRONT_FACE, GL_DIFFUSE, material_Kd);
        glutSolidCube(0.5f);
        glPopMatrix();
    }
    
    glPushMatrix();
    frametorus.ApplyActorTransform();
    //glTranslatef(0.0f, 0.0f, -2.5f);
    if(nShadow == 0)
    {
        glColor3f(1.0f, 1.0f, 0.0f);
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, fBrightLight);
    }
    glPushMatrix();
    //gltDrawUnitAxes();
    glScalef(4.0,6.472,3.0);
    glutSolidCube(0.2f);
    glPopMatrix();
    
    
    glTranslated(-0.25f, -0.65f, 0.0f);
    
    //right feet
    glPushMatrix();
    glutSolidSphere(0.03f, 15, 15);
    
    glTranslated(0.0f, -0.2f, 0.0f);
    glRotatef(rightthigh , 1, 0, 0);
    
    glPushMatrix();
    if(nShadow == 0)
        glColor3ub(0 ,0 ,128);
    glScalef(1.5,2.5,1.0);
    glutSolidCube(0.2f);
    
    glTranslated(0.0f, -0.1, 0.0f);
    glPushMatrix();
    if(nShadow == 0)
        glColor3ub(254 ,232 ,201);
    glRotatef(rightknee, 1.0, 0.0, 0.0);
    glutSolidSphere(0.02f, 15, 15);
    glTranslated(0.0f, -0.1f, 0.0f);
    
    glPushMatrix();
    glRotatef(rightknee, 1.0, 0.0, 0.0);
    //gltDrawUnitAxes();
    glScalef(1.5,2.0,1.0);
    glutSolidCube(0.1f);
    glPopMatrix();
    glPopMatrix();
    
    glPopMatrix();
    
    
    glPopMatrix();
    //end right feet
    
    glTranslated(0.5f, 0.0f, 0.0f);
    
    //left feet
    glPushMatrix();
    if(nShadow == 0)
        glColor3ub(0 ,0 ,128);
    glutSolidSphere(0.03f, 15, 15);
    glTranslated(0.0f, -0.2f, 0.0f);
    glRotatef(leftthigh , 1, 0, 0);
    
    glPushMatrix();
    
    glScalef(1.5,2.5,1.0);
    glutSolidCube(0.2f);
    
    glTranslated(0.0f, -0.1, 0.0f);
    glPushMatrix();
    if(nShadow == 0)
        glColor3ub(254 ,232 ,201);
    glRotatef(leftknee, 1.0, 0.0, 0.0);
    glutSolidSphere(0.02f, 15, 15);
    glTranslated(0.0f, -0.1f, 0.0f);
    
    glPushMatrix();
    //gltDrawUnitAxes();
    glScalef(1.5,2.0,1.0);
    glutSolidCube(0.1f);
    glPopMatrix();
    glPopMatrix();
    
    glPopMatrix();
    
    glPopMatrix();
    //end left feet
    
    glTranslated(0.2, 0.9, 0.0);
    
    //left hand
    glPushMatrix();
    if(nShadow == 0)
        if(nShadow == 0)
        {
            glColor3f(1.0f, 1.0f, 0.0f);
            glMaterialfv(GL_FRONT, GL_SPECULAR, fBrightLight);
        }

    glutSolidSphere(0.03f, 15,15);
    glTranslated(0.1, 0.0, 0.0);
    glRotatef(leftshoulder, x, 0, y);
    
    glPushMatrix();
    glScalef(1.2,2.5,1.0);
    glutSolidCube(0.2f);
    
    
    glTranslated(0.0f, -0.2f, 0.0f);
    glRotatef(leftarm, x, 0, y);
    
    glPushMatrix();
    if(nShadow == 0)
        glColor3ub(254 ,232 ,201);
    glScalef(1.2,2.5,1.0);
    glutSolidCube(0.1f);
    glPopMatrix();
    
    glPopMatrix();
    
    
    glPopMatrix();
    //end left hand
    
    glTranslated(-0.9, 0.0, 0.0);
    
    //right hand
    glPushMatrix();
    if(nShadow == 0)
        glColor3f(1.0f, 1.0f, 0.0f);
    glutSolidSphere(0.03f, 15,15);
    glTranslated(-0.1, 0.0, 0.0);
    glRotatef(rightshoulder, x, 0, y);
    
    glPushMatrix();
    glScalef(1.2,2.5,1.0);
    glutSolidCube(0.2f);
    
    
    glTranslated(0.0f, -0.2f, 0.0f);
    glRotatef(rightarm, x, 0, y);
    
    glPushMatrix();
    if(nShadow == 0)
    {
        glColor3ub(254 ,232 ,201);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, fBrightLight1);
    }
    glScalef(1.2,2.5,1.0);
    glutSolidCube(0.1f);
    
    glPopMatrix();
    
    glPopMatrix();
    
    glPopMatrix();
    //end right hand
    
    
    glTranslated(0.45, 0.4, 0.0);
    
    //head
    if(nShadow == 0)
        glColor3ub(254 ,232 ,201);
    
    glPushMatrix();
    //gltDrawUnitAxes();
    //glColor3f(0.0, 0.5f, 0.0f);
    glutSolidSphere(0.03f, 15,15);
    glTranslated(0.0f, 0.4f, 0.0f);
    glutSolidSphere(0.5f, 15, 15);
    glRotatef(head, 0.0, 1.0, 0.0);
    //printf("%d\n" , head);
    
    glTranslated(0.0f, 0.0f,0.5f);
    glRotated(90, 1, 0, 0);
    
    glPushMatrix();
    //triangle
    glBegin(GL_TRIANGLES);
    if(nShadow == 0)
        glColor3ub(254 ,232 ,201);
    //front triangle
    //glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.15f, 0.0f);
    glVertex3f( -0.15f, -0.15f, 0.0f);
    glVertex3f( 0.1f,  -0.15f, 0.0f);
    
    //right side triangle
    //glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
    glVertex3f( 0.15f,  -0.15f, 0.0f);
    glVertex3f(0.0f, 0.15f, 0.0f);
    glVertex3f( 0.0f,  -0.15f, -0.15f);
    
    //left side triangle
    //glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glVertex3f( -0.15f, -0.15f, 0.0f);
    glVertex3f(0.0f, 0.15f, 0.0f);
    glVertex3f( 0.0f,  -0.15f, -0.15f);
    
    //bottom triangle
    //glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
    glVertex3f( -0.15f, -0.15f, 0.0f);
    glVertex3f( 0.15f,  -0.15f, 0.0f);
    glVertex3f( 0.0f,  -0.15f, -0.15f);
    
    glEnd();
    //end triangle
    glPopMatrix();
    
    glPopMatrix();
    //end head
    
    glMaterialfv(GL_FRONT, GL_SPECULAR, fBrightLight);
    glPopMatrix();
}

// Called to draw scene
void RenderScene(void)
{
    
    // Clear the window with current clearing color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glPushMatrix();
    frameCamera.ApplyCameraTransform();
    
    glPushMatrix();
    glRotated(spin, 1.0, 0.0, 0.0);
    // Position light before any other transformations
    glLightfv(GL_LIGHT0, GL_POSITION, fLightPos);
    glutSolidSphere(0.1f, 13, 26);
    glPopMatrix();
    
    glPushMatrix();
    glLightfv(GL_LIGHT1, GL_POSITION, fLightPos1);
    glPopMatrix();
    
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


void jump (int i)
{
    x = 1;
    if(i < 25)
    {
        if(i<5)
        {
            frametorus.TranslateLocal(0.0f, -0.1f, 0.0f);
            leftthigh = leftthigh - 9;
            rightthigh = rightthigh - 9;
            leftknee+=12;
            rightknee+=12;
            rightshoulder-=6;
            leftshoulder-=6;
            rightarm-=18;
            leftarm-=18;
        }
        else if(i< 10)
        {
            int j = i%5;
            frametorus.TranslateLocal(0.0f, 0.2f , 0.0f);
            leftthigh = leftthigh + 9;
            rightthigh = rightthigh + 9;
            leftknee-=12;
            rightknee-=12;
            rightshoulder+=6;
            leftshoulder+=6;
            rightarm+=18;
            leftarm+=18;
        }
        else if(i<15)
        {
            frametorus.TranslateLocal(0.0f, -0.1f, 0.0f);
        }
        else if(i < 20)
        {
            frametorus.TranslateLocal(0.0f, -0.1f, 0.0f);
            leftthigh-=9;
            rightthigh-=9;
            leftknee+=12;
            rightknee+=12;
            rightshoulder-=6;
            leftshoulder-=6;
            rightarm-=18;
            leftarm-=18;
        }
        else
        {
            frametorus.TranslateLocal(0.0f, 0.1f, 0.0f);
            leftthigh+=9;
            rightthigh+=9;
            leftknee-=12;
            rightknee-=12;
            rightshoulder+=6;
            leftshoulder+=6;
            rightarm+=18;
            leftarm+=18;
            
        }
        i++;
        glutTimerFunc(100, jump , i );
    }
    glutPostRedisplay();
}

void walk(int i)
{
    x = 1;
    if(i < 9)
    {
        if(i < 3)
        {
            frametorus.TranslateLocal(0.0f, 0.0f, 0.1f);
            rightthigh-=10;
            rightknee+=10;
            leftthigh+=10;
            leftshoulder-=5;
            leftarm-=5;
            rightshoulder+=5;
            rightarm+=5;
        }
        else if (i < 6)
        {
            frametorus.TranslateLocal(0.0f, 0.0f, 0.1f);
            leftthigh-=20;
            leftknee+=10;
            rightthigh+=20;
            leftshoulder+=10;
            leftarm+=10;
            rightshoulder-=10;
            rightarm-=10;
        }
        else
        {
            frametorus.TranslateLocal(0.0f, 0.0f, 0.1f);
            rightthigh-=10;
            leftthigh+=10;
            rightknee-=10;
            leftknee-=10;
            leftshoulder-=5;
            leftarm-=5;
            rightshoulder+=5;
            rightarm+=5;
        }
        i++;
        glutTimerFunc(100, walk, i);
    }
    glutPostRedisplay();
}
void wave(int i)
{
    if(i < 12)
    {
        if(i < 3)
        {
            x=1;
            y=0;
            rightshoulder-=15;
            rightarm-=30;
        }
        else if(i < 9)
        {
            x = 0;
            y = 1;
            if(i%2 == 1)
                rightarm -= 30;
            else
                rightarm += 30;
            
        }
        else
        {
            x = 1;
            y= 0 ;
            rightshoulder+=15;
            rightarm+=30;
        }
        
        i++;
        glutTimerFunc(100, wave, i);
    }
    glutPostRedisplay();
}

void clap(int i)
{
    if (i < 9)
    {
        if(i < 3)
            head +=30;
        else if (i < 6)
            head -= 60;
        else
            head+=30;
        
        i++;
        glutTimerFunc(100, clap, i);
    }
    glutPostRedisplay();
}

void rotateCube( int i )
{
    if( i < 10 )
    {
        if(CubeAngle>360)
            CubeAngle = 0;
        else
            CubeAngle +=15;
        
        i++;
        glutTimerFunc(100, rotateCube , i);
    }
    glutPostRedisplay();
}

void movelight(int i)
{
    //if(i < 10)
    {
    spin = (spin + 3) % 360;
    //i++;
    glutTimerFunc(1000, movelight, 1);
    //printf("%d\n" , spin);
    }
    glutPostRedisplay();
}

void myKeys(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 'g':
            movelight(1);
            break;
        case 't':
            rotateCube(0);
            break;
        case 'j':
            jump(0);
            break;
            //    case 'k':
            //            walk(0);
            //            break;
        case 'p':
            wave(0);
            break;
        case 'o':
            clap(0);
            break;
            
        case 'w':
            /* Your Implementation */
            frametorus.MoveForward(0.1f);
            walk(0);
            break;
        case 's':
            /* Your Implementation */
            frametorus.RotateLocalY(3.15f);
            break;
        case 'a':
            /* Your Implementation */
            frametorus.RotateLocalY(0.1f);
            break;
        case 'd':
            /* Your Implementation */
            frametorus.RotateLocalY(-0.1f);
            break;
        case 'u':
            frameCamera.MoveUp(0.1f);
            break;
        case 'l':
            frameCamera.MoveUp(-0.1f);
            break;
            
        case 'b':
            bf(0);
        break;    }
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
    movelight(1);
    
    glutTimerFunc(33, TimerFunction, 1);
    
    
    glutMainLoop();
    
    return 0;
}
