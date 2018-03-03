#include <iostream>
#include <GL/glut.h>

#include "chunk.h"

Chunk<BlockType, 4> c;

void display();

int main(int argc, char** argv)
{

  //init glut lib. Use this temporary for testing while we dont switch to
  //modern ogl and qt
  glutInit(&argc, argv);

  //init window
  glutInitWindowSize(1600, 900);
  glutInitWindowPosition(0, 0);
  glutCreateWindow("RS PROJECT");

  //now that window is created
  glutInitDisplayMode(GL_DOUBLE | GL_RGB | GL_DEPTH);

  //init drawing state
  glColor3f(1., 0., 0.);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60, 16 / (float) 9, 1, 100);
  gluLookAt(10, 10, 10, 0, 0, 0, 0, 1, 0);

  //callbacks
  glutDisplayFunc(display);

  glutMainLoop();

  return 0;
}

void display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  c.drawAll();

  glutSwapBuffers();
  glutPostRedisplay();
}
