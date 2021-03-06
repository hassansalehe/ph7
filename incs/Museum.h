///////////////////////////////////////////////////////////////////////
//
//                   COMP 510, Computer Graphics, Spring 2016
//                              Final project
//                PH7: A virtual Museum Based on OpenGL and Glut
//
//                          (c) 2016,2017,2018 - Hassan & Pirah.
//          Copying without the authors consent is strictly prohibited.
//
///////////////////////////////////////////////////////////////////////
//
// Implements the museum class wich holds all the museum objects in
// form of a tree. Moreover, it contains all necessary functions to
// manipulate these objects. See Init.cpp for how the "Museum" class
// is used.
//
///////////////////////////////////////////////////////////////////////

#ifndef MUSEUM_HPP
#define MUSEUM_HPP

// queue for rendering objects
#include <queue>

#include "Object.h"
#include "Room.h"
#include "Roof.h"
#include "DoorFrame.h"
#include "Header.h"
#include "Wframe.h"
#include "Stand.h"
#include "Sun.h"
#include "Skull.h"
#include "Part.h"
#include "Walkman.h"
#include "Wheel.h"
#include "Airplane.h"
#include "Lamp.h"

#include "RearUpperFace.h"

class Museum {

private:
  Object **objects;     // pointer to hold objects
  GLuint   objectCount; // total number of objects
  GLuint   program;     // pointer to program

  // queue to process the object tree
  std::queue<Object *> objectQueue;

  // the top object in the tree
  Object *root;


  /**
   * Callback to construct the objects and the tree
   * Each new museum object is added here.
   */
  void constructObjectTree() {

    // Create objects
    Object *room       = new Room();
    Object *roof       = new Roof();
    Object *doorframe  = new DoorFrame();
    Object *frontUpper = new Header();
    Object *rearUpper  = new Cube();

    // right windows
    Object *rfwindow  = new WindowFrame(point4(0.5, 0.1, 0.35, 1.0), 90);
    Object *rrwindow  = new WindowFrame(point4(0.5, 0.1, -0.35, 1.0), 90);

    // left windows
    Object *lfwindow  = new WindowFrame(point4(-0.5, 0.1, 0.35, 1.0), -90);
    Object *lrwindow  = new WindowFrame(point4(-0.5, 0.1, -0.35, 1.0), -90);

    // rear windows
    Object *reRightwindow  = new WindowFrame(point4( 0.3,  0.33, -0.8, 1.0), vec3(0.4, 0.35, 1.0));
    Object *reMiddlewindow = new WindowFrame(point4(0.0,  0.33, -0.8, 1.0), vec3(0.4, 0.35, 1.0));
    Object *reLeftwindow   = new WindowFrame(point4( -0.3,  0.33, -0.8, 1.0), vec3(0.4, 0.35, 1.0));

    Object *stand    = new Stand();
    Object *sun      = new  Sun();
    Object *skull    = new Skull();
    Object *walkman  = new Walkman();
    Object *part     = new Part();
    Object *wheel    = new Wheel();
    Object *airplane = new Airplane();
    Object *lamp     = new Lamp();

    // Construct tree
    room->appendChild( roof );
    room->appendChild( doorframe );
    room->appendChild( frontUpper );
    room->appendChild( rearUpper );

    // side windows
    room->appendChild( rfwindow ); // right front WindowFrame
    room->appendChild( rrwindow ); // right rear WindowFrame

    room->appendChild( lfwindow ); // left front WindowFrame
    room->appendChild( lrwindow ); // left rear WindowFrame

    //rear windows
    room->appendChild( reRightwindow );
    room->appendChild( reMiddlewindow );
    room->appendChild( reLeftwindow );

    room->appendChild( stand );
    room->appendChild( sun );

    // children of stand
    stand->appendChild( skull );
    stand->appendChild( walkman );
    stand->appendChild( part );
    stand->appendChild( wheel );
    stand->appendChild( airplane );
    stand->appendChild( lamp );

    // set root to the room object
    root = room;
  }


  /**
   * Callback to initialize the tree objects
   */
  void initializeTreeNodes() {

    // put the root in the queue
    objectQueue.push(root);

    while ( !objectQueue.empty() ) {
      Object *object = objectQueue.front();
      objectQueue.pop();
      object->initialize( program );
      object->pushChildrenToQueue( objectQueue );
    }
  }


  /**
   * Callback to display objects in the tree
   */
  void displayObjects() {

    // put the root in the queue
    objectQueue.push(root);

    while ( !objectQueue.empty() ) {
      Object *object = objectQueue.front();
      objectQueue.pop();

      object->display( program ); // display
      object->sendModeViewToChildren();
      object->pushChildrenToQueue( objectQueue );
    }
  }


public:

  /**
   * Initializes the objects of the museum.
   * It calls functions to create the tree nodes(objects)
   */
  void initialize() {

    // Load shaders and use the resulting shader program
    program = InitShader( "shaders/MuseumVshader.glsl",
                          "shaders/MuseumFshader.glsl" );

    objectCount = 0;
    objects     = new Object*[objectCount];

    constructObjectTree();
    initializeTreeNodes();
    displayObjects();

    // set sky blue
    // Set the state variable "clear color" to clear buffer
    glClearColor(
        0.52941176470588235294,
        0.80784313725490196078,
        0.98039215686274509804,
        1.0
    );

   // tell user to retrieve help message
   std::cout << "Type 'h' or 'H' for full information ";
   std::cout << "on how to interact with the museum" << std::endl;
  }

  /**
   * This stub calls all display functions of the objects
   */
  void display() {
    displayObjects();
  }


  /**
   * Launches the idle functions of each object
   */
  void idle() {

    objectQueue.push(root);

    while ( !objectQueue.empty() ) {
      Object *object = objectQueue.front();
      objectQueue.pop();
      object->idle(); // call idle of each object
      object->pushChildrenToQueue( objectQueue );
    }
  }


  /**
   * The reshape function
   */
  void reshape(int w, int h) {

    objectQueue.push(root);

    while ( !objectQueue.empty() ) {
      Object *object = objectQueue.front();
      objectQueue.pop();
      object->reshape( w, h ); // reshape
      object->pushChildrenToQueue( objectQueue );
    }
  }

  /**
   * Propagates the rotation angle along y-axis to all the museum
   * objects.
   */
  void rotateLeft(GLfloat delta) {
    root->rotateLeft( delta );
    displayObjects();
  }


  /**
   * Propagates the rotation angle along x-axis to all the museum
   * objects.
   */
  void rotateUp(GLfloat delta) {

    objectQueue.push(root);

    while ( !objectQueue.empty() ) {
      Object *object = objectQueue.front();
      objectQueue.pop();
      object->rotateUp( delta ); // rotate
      object->pushChildrenToQueue( objectQueue );
    }
  }


  void zoomOut(GLfloat delta) {

    objectQueue.push(root);

    while ( !objectQueue.empty() ) {
      Object *object = objectQueue.front();
      objectQueue.pop();
      object->zoomOut( delta ); // zoom
      object->pushChildrenToQueue( objectQueue );
    }
  }


  void zoomIn(GLfloat delta) {

    objectQueue.push(root);

    while ( !objectQueue.empty() ) {
      Object *object = objectQueue.front();
      objectQueue.pop();
      object->zoomIn( delta ); // zoom
      object->pushChildrenToQueue( objectQueue );
    }
  }


  /**
   * Move the viewer in forward direction in the museum -z Direction
   */
  void moveForward(GLfloat delta) {

    objectQueue.push(root);

    while ( !objectQueue.empty() ) {
      Object *object = objectQueue.front();
      objectQueue.pop();
      object->moveForward( delta ); // move
      object->pushChildrenToQueue( objectQueue );
    }
  }
  /*
   * Toggle auto rotation
   */
  void toggleAuto() {

    objectQueue.push(root);

    while ( !objectQueue.empty() ) {
      Object *object = objectQueue.front();
      objectQueue.pop();
      object->toggleAuto(); // auto rotate
      object->pushChildrenToQueue( objectQueue );
    }
  }


  /**
   * Function for reseting the museum to its initial
   * position and orientation
   */
  void reset() {

    objectQueue.push(root);

    while ( !objectQueue.empty() ) {
      Object *object = objectQueue.front();
      objectQueue.pop(); // reset
      object->reset();
      object->pushChildrenToQueue( objectQueue );
    }
  }


  void changeShading() {
    root->changeShading();
  }

  void changeReflection() {
    root->changeReflection();
  }


  /**
   * Broadcasts the pixel picked on mouse click
   */
  void broadcastSelectedPixel(unsigned char pixel[4]) {

    objectQueue.push(root);

    while ( !objectQueue.empty() ) {
      Object *object = objectQueue.front();
      objectQueue.pop();
      object->checkIfPicked( pixel ); // check if picking
      object->pushChildrenToQueue( objectQueue );
    }
  }


  /**
   * Launches the idle functions of each object for appropriate
   * actions once one of the objects is picked.
   */
  void enablePicking() {

    objectQueue.push(root);

    while ( !objectQueue.empty() ) {
      Object *object = objectQueue.front();
      objectQueue.pop();
      object->enablePicking(); // enable picking
      object->pushChildrenToQueue( objectQueue );
    }
  }

    /**
   * Launches the idle functions of each object
   */
  void disablePicking() {

    objectQueue.push(root);

    while ( !objectQueue.empty() ) {
      Object *object = objectQueue.front();
      objectQueue.pop();
      object->disablePicking(); // disable picking
      object->pushChildrenToQueue( objectQueue );
    }
  }


  /**
   * For toggling projection(perspective vs orthoginal)
   */
  void changeProjection() {
    root->changeProjection();
  }

  /**
   * Prints the help information to the user.
   * It also launches help methods of individual objects
   */
  void help() {
    cout << "================================================================" << endl;
    cout << "=                                                              =" << endl;
    cout << "= Welcome to PH7: The Virtual museum developed only on OpenGL. =" << endl;
    cout << "=                                                              =" << endl;
    cout << "================================================================" << endl;
    cout << "=                                                              =" << endl;
    cout << "=         HOW TO INTERACT WITH THE MUSEUM                      =" << endl;
    cout << "=                                                              =" << endl;
    cout << "================================================================" << endl;
    cout << "=   Keys         ==             Function                       =" << endl;
    cout << "================================================================" << endl;
    cout << "=  H or h        ==  To get this help message.                 =" << endl;
    cout << "=  Mouse-click   ==  To pick an(any) object.                   =" << endl;
    cout << "=  I or i        ==  To reset the museum                       =" << endl;
    cout << "=  Z             ==  To zoom IN                                =" << endl;
    cout << "=  z             ==  To zoom out                               =" << endl;
    cout << "=  f             ==  To move forward                           =" << endl;
    cout << "=  F             ==  To move backward                          =" << endl;
    cout << "=  S or s        ==  To change shading                         =" << endl;
    cout << "=  A or a        ==  To turn auto rotation on/off of antiques  =" << endl;
    cout << "=                                                              =" << endl;
    cout << "=  Left arrow    ==  To rotate Left                            =" << endl;
    cout << "=  Right arrow   ==  To rotate Right                           =" << endl;
    cout << "=  Up arrow      ==  To rotate up                              =" << endl;
    cout << "=  Down arrow    ==  To rotate down                            =" << endl;
    cout << "=  Click on door ==  To open / close the door                  =" << endl;
    cout << "=  Q or q        ==  To close/quit the museum                  =" << endl;
    cout << "=  Click window  ==  To open/close window (blend)              =" << endl;

    objectQueue.push(root);

    while ( !objectQueue.empty() ) {
      Object *object = objectQueue.front();
      objectQueue.pop();
      object->help(); // call help of each object
      object->pushChildrenToQueue( objectQueue );
    }

    cout << "================================================================" << endl;
  }


  /**
   * destroys all dynamically created objects (memory management)
   */
  void close() {

    objectQueue.push(root);

    while ( !objectQueue.empty() ) {
      Object *object = objectQueue.front();
      objectQueue.pop();
      object->disablePicking(); // disable picking
      object->pushChildrenToQueue( objectQueue );

      delete object;
    }
  }
};

#endif // end of museum_hpp
