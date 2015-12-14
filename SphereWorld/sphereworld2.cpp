// SphereWorld.cpp
// OpenGL SuperBible
// Demonstrates an immersive 3D environment using actors
// and a camera.
// Program by Richard S. Wright Jr.

#include "gltools.h"	// OpenGL toolkit
#include "glframe.h"   // Frame Class
#include "math3d.h"
#include <math.h>

#define NUM_SPHERES      30
GLFrame    spheres[NUM_SPHERES];
GLFrame    frameCamera;
GLFrame    frametorso;

//+ Light and material Data
GLfloat fLightPos[4]   = { -100.0f, 100.0f, 50.0f, 1.0f };  // Point source
GLfloat fNoLight[] = { 0.0f, 0.0f, 0.0f, 0.0f };
GLfloat fLowLight[] = { 0.25f, 0.25f, 0.25f, 1.0f };
GLfloat fBrightLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
//+ Light and material Data
GLfloat fLightPos2[4]   = { -200.0f, 200.0f, 50.0f, 1.0f };  // Point source
GLfloat fNoLight2[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat fLowLight2[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat fBrightLight2[] = { 1.0f, 1.0f, 1.0f, 1.0f };
//+ Light and material Data
////////////////////////////////////////////////////////////////////////////////////////////////////

M3DMatrix44f mShadowMatrix;
//+

float delta=0;
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
// This function does any needed initialization on the rendering
// context.

/////////////////////////////////////setMaterial

void SetupRC()
{
    int iSphere;

//+ Calculate shadow matrix
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

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, fNoLight2);
    glLightfv(GL_LIGHT1, GL_AMBIENT, fLowLight2);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, fBrightLight2);
    glLightfv(GL_LIGHT1, GL_SPECULAR, fBrightLight2);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT1);

//+ Get the plane equation from three points on the ground
     M3DVector4f vPlaneEquation;
    m3dGetPlaneEquation(vPlaneEquation, vPoints[0], vPoints[1], vPoints[2]);

    // Calculate projection matrix to draw shadow on the ground
    m3dMakePlanarShadowMatrix(mShadowMatrix, vPlaneEquation, fLightPos);


    for(iSphere = 0; iSphere < NUM_SPHERES; iSphere++)
    {
        // Pick a random location between -20 and 20 at .1 increments
        float x = ((float)((rand() % 400) - 200) * 0.1f);
        float z = (float)((rand() % 400) - 200) * 0.1f;
        spheres[iSphere].SetOrigin(x, 0.0f, z);
    }

    frametorso.SetOrigin(0.0f, -0.6f, -17.5);
}


///////////////////////////////////////////////////////////
// Draw a gridded ground
void DrawGround(void)
{
    GLfloat fExtent = 40.0f;
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
	    frametorso.ApplyActorTransform();
	    //glTranslatef(0.0f, 0.0f, -2.5f);
        if(nShadow == 0)
            glColor3f(0.0f, 0.0f, 1.0f);

        glPushMatrix();
            glRotatef(-yRot * 2.0f, 0.0f, 1.0f, 0.0f);
            glTranslatef(1.0f, 0.0f, 0.0f);
    		glutSolidSphere(0.1f, 13, 26);
        glPopMatrix();

        if(nShadow == 0)
            {
            // Torus alone will be specular
            glColor3f(1.0f, 0.0f, 0.0f);
            glMaterialfv(GL_FRONT, GL_SPECULAR, fBrightLight);
            }
        glPushMatrix();
        glRotatef(yRot, 0.0f, 1.0f, 0.0f);
         DrawRobot();
        glMaterialfv(GL_FRONT, GL_SPECULAR, fNoLight);
    glPopMatrix();
    }

//+ Called to draw scene
void RenderScene(void)
    {
   // Clear the window with current clearing color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
   /* glPushMatrix();
        glFrustum(-1.00,1.00,-1.00,1.00,0.50,3.50);
        gluLookAt(0.00,0.00,2.00,
                  0.00,0.00,0.00,
                  0.00,1.00,0.00);*/
        frameCamera.ApplyCameraTransform();
  // glPopMatrix();
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
// Called to draw scene
/*void RenderScene(void)
{
    int i;
    static GLfloat yRot = 0.0f;         // Rotation angle for animation
    yRot += 0.5f;

    // Clear the window with current clearing color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    frameCamera.ApplyCameraTransform();

    // Draw the ground
    glColor3f(0.0f, 1.0f, 1.0f);
    DrawGround();

    // Draw the randomly located spheres
    for(i = 0; i < NUM_SPHERES; i++)
    {
        glPushMatrix();
        spheres[i].ApplyActorTransform();
        glRotatef(yRot, 0.0f, 1.0f, 0.0f);
            glPushMatrix();
            if(i<=15){
                glColor3f(1.0f, 0.0f, 0.1f);
            }else{
                 glColor3f(1.0f, 0.0f, 1.0f);
            }
                glTranslatef(0.0f,0.35f,0.0f);
                glutSolidSphere(0.15f, 13, 26);
            glPopMatrix();

			glRotatef(180,1,0,0);
			//glScalef(0.2,0.15,0.4);
			 glScalef(0.15,0.25,0.1);
			 glColor3f(1.0f, 1.0f, 0.0f);
            pyramid();
             glColor3f(1.0f, 1.0f, 1.0f);
        glPopMatrix();
    }
    // Draw the robot
    DrawRobot();

    // Do the buffer Swap
    glutSwapBuffers();
}*/
void jump(int index){
    if(index<40){
        if(index==0){
            left_thigh=right_thigh=0;
            left_calf=right_calf=0;
            leftup_arm=rightup_arm=0;
            leftdown_arm=rightdown_arm=0;
        }
        if(index<10){
             xtorso=-10;
            left_calf-=10;
            right_calf-=10;
            leftdown_arm+=5;
            rightdown_arm+=5;
            frametorso.TranslateLocal(0.0f,0.2,0.0f);
            index++;
        }
        if(index>=10 && index<20){
                 xtorso=10;
                leftup_arm+=15;
                rightup_arm+=15;
                leftdown_arm-=5;
                rightdown_arm-=5;
            if(index<15){
                frametorso.TranslateLocal(0.0f,0.2,0.0f);
        }
            index++;
        }
        //fall down upper leg
                if(index>=20 && index<30){
                    leftup_arm-=15;
                    rightup_arm-=15;
                    leftdown_arm+=5;
                    rightdown_arm+=5;
                    left_calf+=5;
                    right_calf+=5;
                    if(index>=25 && index<30){
                        frametorso.TranslateLocal(0.0f,-0.2,0.0f);
                    }
                    index++;
                }
        //fall down lower leg
                if(index>=30 && index<40){
                    xtorso=0;
                    frametorso.TranslateLocal(0.0f,-0.2,0.0f);
                    left_calf+=5;
                    right_calf+=5;
                    leftdown_arm-=5;
                    rightdown_arm-=5;
            index++;
        }
        glutTimerFunc(30,jump,index);
    }
    glutPostRedisplay();

}
void walk(int index)
{

        if(index<30){
            if(index<5){ // 右腳往前
                frametorso.TranslateLocal(0.0f,0.0f,-0.05f);
                right_thigh+=10;
                right_calf-=20;
                leftup_arm+=5;
                xtorso-=1;
                index++;
            }
            if(index>=5 && index<10){
                 frametorso.TranslateLocal(0.0f,0.0f,-0.05f);
                if(index==5){ // 右腳放下
                    right_thigh=0;
                    right_calf=0;
                    leftup_arm=0;
                    xtorso=0;
                }
            left_thigh+=10; // 左腳往前
            left_calf-=20;
            rightup_arm+=5;
            xtorso-=1;

                if(index==10){ // 左腳放下
                    left_thigh=0;
                    left_calf=0;
                    rightup_arm=0;
                    xtorso=0;
                }
            index++;
        }
            if(index>10 && index<15){
                frametorso.TranslateLocal(0.0f,0.0f,-0.05f);
                right_thigh+=10;
                right_calf-=20;
                leftup_arm+=5;
                xtorso-=1;
                index++;
            }
            if(index>=15 && index<20){
                 frametorso.TranslateLocal(0.0f,0.0f,-0.05f);
                if(index==15){
                    right_thigh=0;
                    right_calf=0;
                    leftup_arm=0;
                    xtorso=0;
                }
            left_thigh+=10;
            left_calf-=20;
            rightup_arm+=5;
            xtorso-=1;

                if(index==20){
                    left_thigh=0;
                    left_calf=0;
                    rightup_arm=0;
                    xtorso=0;
                }
            index++;
        }
    glutTimerFunc(500, walk, index);
   }

    glutPostRedisplay();
}
///////////////////action of the robot
void knockHead(){
    tmp=(tmp+1)%15;
        if(tmp<=5)
			xneck = tmp;
        else
            xneck = 10-tmp;
         glutPostRedisplay();
}
void fly(int index){
    if(index<400){
        if(index==0){
            left_thigh=right_thigh=0;
            left_calf=right_calf=0;
            leftup_arm=rightup_arm=0;
            leftdown_arm=rightdown_arm=0;
            for(int i=0;i<20;i++){
            frametorso.RotateLocalY(-0.1f);
           }
        }
        if(index<10){
            left_thigh=right_thigh=-25;
            left_calf=right_calf=-90;
        if(index==9){
            xtorso=(xtorso-5) % 90;
        }
        index++;
       }
        if(index>=10 && index<19){
            xtorso=(xtorso-5) % 90;
            index++;
        }
        if(index>=19 && index<25){
            rightup_arm=130;
            leftdown_arm=45;
            left_thigh=right_thigh=-25;
            left_calf=right_calf=-25;
            index++;
        }
        if(index>=25 && index<350){
            leftup_arm=130;
            leftdown_arm=0;
            frametorso.MoveForward(-0.1f);
           /* if(index>=100 && index<=115){
            frametorso.TranslateLocal(0.0f,0.2,0.0f);
            }*/
             if(index>=50 && index<100){
                frametorso.TranslateLocal(0.0f,0.09,0.0f);
            }
             if(index>=200 && index<250){
                frametorso.TranslateLocal(0.0f,-0.09,0.0f);
            }
            index++;
        }
        if(index>=350 && index<355){
            xtorso=(xtorso+5) % 90;
            index++;
        }
        if(index>=355 && index<365){
            left_thigh=right_thigh=25;
            left_calf=right_calf=25;
            index++;
        }
        if(index>=365 && index<370){
            xtorso=0;
            left_thigh=right_thigh=0;
            left_calf=right_calf=0;
            index++;
        }
        if(index>=370 && index<375){
            rightup_arm=90;
            index++;
        }
        if(index>=375 && index<380){
            rightup_arm=0;
            leftup_arm=0;
            index++;
        }
     glutTimerFunc(33, fly, index);
}
   glutPostRedisplay();
}

//////////////////////////////////
void myKeys(unsigned char key, int x, int y)
{
    switch(key)
    {
    case 'w':
        /* Your Implementation */
          frametorso.MoveForward(-0.1f);
        //Ex:  frametorus.????
        break;
    case 's':
        /* Your Implementation */
         frametorso.MoveForward(0.1f);
        break;
    case 'a':
        /* Your Implementation */
         frametorso.RotateLocalY(-0.1f);
        break;
    case 'd':
        /* Your Implementation */
        frametorso.RotateLocalY(0.1f);
        break;
    case '0':
        knockHead();
        break;
    case '1':
        rightup_arm = (rightup_arm + 5) % 360;
        glutPostRedisplay();
		 break;
    case '2':
        rightdown_arm = (rightdown_arm+5) % 100;
        glutPostRedisplay();
         break;
    case '3':
        leftup_arm = (leftup_arm + 5) % 360;
        glutPostRedisplay();
		 break;
    case '4':
        leftdown_arm = (leftdown_arm + 5) % 100;
        glutPostRedisplay();
         break;
    case '5':
        right_thigh = (right_thigh +5) %70;
        glutPostRedisplay();
         break;
    case '6':
        right_thigh = (right_thigh +5) %35;
        right_calf = (right_calf -10) %90;
        glutPostRedisplay();
         break;
    case '7':
        left_thigh = (left_thigh +5) %70;
         glutPostRedisplay();
         break;
    case '8':
        left_thigh = (left_thigh +5) %35;
        left_calf = (left_calf -10) %90;
        glutPostRedisplay();
        break;
    case 'f':
      fly(0);
        break;
  /*  case 'r':
        walk(0);
        break;*/
    case 'j':
        jump(0);
        break;
    case 'q':
        exit(0);
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
        frameCamera.RotateLocalY(0.1);

    if(key == GLUT_KEY_RIGHT)
        frameCamera.RotateLocalY(-0.1);

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
    glutCreateWindow("OpenGL Robot Demo");
    glutReshapeFunc(ChangeSize);
    glutDisplayFunc(RenderScene);
    glutKeyboardFunc(myKeys);
    glutSpecialFunc(SpecialKeys);

    SetupRC();
    glutTimerFunc(33, TimerFunction, 1);

    glutMainLoop();

    return 0;
}
