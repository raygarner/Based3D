#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
#include <stdbool.h>

float angle = 0.0f;//rotation angle for the camera

float lx=0.0f, lz=1.0f; //vector to represent the camera's direction

float rx, rz; //vector to represent the crossproduct of the current camera position

float camx=0.0f, camz=5.0f; //xz position of the camer

//states for the program. when no key is being pressed they will be as they are declared
float deltaAngle= 0.0f;
float deltaMove= 0.0f;
float sdeltaMove = 0.0f; //this is for strafing speed
int xOrigin = -1;
bool moving = false;

void changeSize(int w, int h);
void calcPos(float deltaMove, float sdeltaMove);
void renderScene(void);
void processNormalKeys(unsigned char key, int xx, int yy);
void pressKey(int key, int xx, int yy);
void releaseKey(int key, int x, int y);
void mouseMove(int x, int y);
void mouseButton(int button, int state, int x, int y);
void winInit(void);
void regCallBacks(void);
void drawSphere();
void drawAxis(void);

int main(int argc, char *argv[]){
  
  glutInit(&argc, argv);

  winInit(); //init glut and the window
  
  regCallBacks(); //set up callbacks

  glEnable(GL_DEPTH_TEST); //allows the depth buffer to be used

  glutMainLoop();

  return 0;
}


void winInit(void){
  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA); //use a window that has a depth buffer and is double buffered
  glutInitWindowPosition(100, 100);
  glutInitWindowSize(500,500);
  glutCreateWindow("Based3D");

}

void regCallBacks(void){
  //display callbacks
  glutDisplayFunc(renderScene);
  glutReshapeFunc(changeSize);
  glutIdleFunc(renderScene);

  //keyboard callbacks
  glutIgnoreKeyRepeat(1);
  glutKeyboardFunc(processNormalKeys);
  glutSpecialFunc(pressKey);
  glutSpecialUpFunc(releaseKey);

  //mouse callbacks
  glutMouseFunc(mouseButton);
  glutMotionFunc(mouseMove);

}

void renderScene(void){
  
  if(moving)
    calcPos(deltaMove, sdeltaMove);

  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear colour and depth buffers

  glLoadIdentity(); //reset transformations

  gluLookAt(camx, 1.0f, camz, //eye (camera) xyz --- posx,posy,posz
            camx+lx, 1.0f, camz+lz, //reference point (what you want to look at) xyz --- viewx,viewy,viewz
            0.0f, 1.0f, 0.0f); //normalized "up" vector --- upx, upy, upz
 

  //draw ground
  glColor3f(0.9f, 0.9f, 0.9f);
	glBegin(GL_QUADS);
		glVertex3f(-100.0f, 0.0f, -100.0f);
		glVertex3f(-100.0f, 0.0f,  100.0f);
		glVertex3f( 100.0f, 0.0f,  100.0f);
		glVertex3f( 100.0f, 0.0f, -100.0f);
	glEnd();
  
  drawAxis();

  //draw sphere
       //glPushMatrix();
       glTranslatef(0*10.0,0,0 * 10.0);
       drawSphere();
       //glPopMatrix();
    
  glutSwapBuffers();


}

void calcPos(float deltaMove, float sdeltaMove){
  //simple method of moving forward or backward by moving towards the point at which the camera is looking
  camx += deltaMove * lx * 0.1f; 
  camz += deltaMove * lz * 0.1f;
  
  //method for strafing left and right
  /*
  camx += deltaMove * rx * 0.1f;
  camz += deltaMove * ry * 0.1f;

  camz += deltaMove * upVector * 0.1f; (maybe?)
  */
  
  //rightVector = crossProduct(lx, 1.0, lz, 0.0, 1.0, 0.0)
  //
  
  //crossproduct of look vector and up vector
  rx = 1.0f * 0.0f - lz * 1.0f; 
  rz = lx * 1.0f - 1.0f * 0.0f; 


  camx += sdeltaMove * rx * 0.1f;
  camz += sdeltaMove * rz * 0.1f;
}


  
void changeSize(int w, int h){
  
  if(h==0)
    h = 1;

  float ratio = w * 1.0 / h;

  glMatrixMode(GL_PROJECTION); //selects the projection matrix stack as a target for operations

  glLoadIdentity(); //reset matrix

  glViewport(0,0,w,h); //set the viewport to be the entire window

  gluPerspective(45.0f, ratio, 0.1f, 100.0f); //sets the correct perspective

  glMatrixMode(GL_MODELVIEW); //sets the target matrix back to the model matrix (default)

}

void processNormalKeys(unsigned char key, int xx, int yy){
  if (key == 27) //if esc is pressed
    exit(0);

}

void pressKey(int key, int xx, int yy){
  switch(key){
    
    case GLUT_KEY_UP : deltaMove = 0.5f; moving = true; break;

    case GLUT_KEY_RIGHT : sdeltaMove = 0.5f; moving = true;break;

    case GLUT_KEY_LEFT : sdeltaMove = -0.5f; moving = true;break;

    case GLUT_KEY_DOWN : deltaMove = -0.5f; moving = true;break;
  }

}

void releaseKey(int key, int x, int y){
  switch(key){
    case GLUT_KEY_UP:
    case GLUT_KEY_DOWN: deltaMove = 0; break;
  }

  switch (key){
    case GLUT_KEY_RIGHT:
    case GLUT_KEY_LEFT: sdeltaMove = 0; break;

  }
}

void mouseButton(int button, int state, int x, int y){
  
  if(button == GLUT_LEFT_BUTTON){ //when mouse clicked
    if(state == GLUT_UP){ //when lmouse released
      angle += deltaAngle;
      xOrigin = -1;

    }else
      xOrigin = x;

  }

}

void mouseMove(int x, int y){
  if(xOrigin >0){ //when lmouse down
    deltaAngle = (x - xOrigin) * 0.001f; //update deltaAngle

    //update cam direction
    lx = sin(angle + deltaAngle);
    lz = -cos(angle + deltaAngle);


  
  }

}

void drawSphere(){
  glColor3f(1.0f, 0.0f, 0.0f);

  glTranslatef(0.0f, 0.7f, 0.0f); //raise the the shape so it doesnt clip through the floor
  glutSolidSphere(0.25f,20,20); //radius, slices and stacks
}

void drawAxis(){
  float len = 100;

  glColor3f(0.0,0.0,1.0);

  glBegin(GL_LINES);
    glVertex3f(0,0.1,0);
    glVertex3f(len,0.1,0);
    glVertex3f(0,0.1,0);
    glVertex3f(0,len,0);
    glVertex3f(0,0.1,0);
    glVertex3f(0,0.1,len);
  glEnd();

}

