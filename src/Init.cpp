///////////////////////////////////////////////////////////////////////////////
//
//                   COMP 510, Computer Graphics, Spring 2016
//                              Final project
//                PH7: A virtual Museum Based on OpenGL and Glut
//
//                            (c) 2016 - Hassan & Pirah.
//            Copying without the authors consent is strictly prohibited.
//
///////////////////////////////////////////////////////////////////////////////
//
//   This is file implements the initialization function of the program.
//
///////////////////////////////////////////////////////////////////////////////

#include "Init.h"
#include "Room.h"

Room room;

void PH7initialize() {

  room.initialize();
  room.display();
}

void displayMuseum() {
  room.display();
}

void idleRedisplay() {
  room.idle();
}
