## Introduction
This is the main repository for the project PH7. PH7 is a virtual museum based on OpenGL and GLut.

## The team members in this project are:
   1. Hassan Salehe Matar
   2. Pirah Noor Soomro

## Licence
More licence information for the project will be added here.
  (c) 2016 - Hassan & Pirah.

## The file structure of the project is as follows:

  - _ README.txt        This file you are currently reading.
  - _ Makefile:         The Main make file of the project.
  - _ TODO.txt          The current ToDo list.
  -   MuseumFshader.glsl The fragments shader of the museum.
  - MuseumVshader.glsl   The vertex shader of the museum.
  - _ docs:              The directory containing documentation files of the project.
  - _ incs:              The directory for the include files.
  - _ _ _ _ _ Init.h       The header file for functions implemented in Init.cpp.
  - _ _ _ _ _ Keyboard.h   The header file for mouse and Keyboard functions.
  - _ _ _ _ _ Object.h     Contains the base class for all museum objects.
  - _ _ _ _ _ Roof.h       Implements the roof of the museum.
  - _ _ _ _ _ Room.h       Implements the room of the museum.
  - _ _ _ _ _ Museum.h       The header file for functions necessary for rendering all the objects.
  - _ src:               The directory for the source files.
  - _ _ _ _ _ Init.cpp     Source code for initializing the shader programs.
  - _ _ _ _ _ ph7Main.cpp  Source code containing the main function for OpenGL and Glut.
  -           Keyboard.cpp Implements the mouse and Keyboard events callbacks.
  - _ libs:              The directory for any librarys and third part source files.
  - _ bin:              The binary file that holds the executables built by the make file.

## How to build this project
There is a make file at the main directory of the project. To build the project run the command `make`. This produces the executable named `ph7main`. 

## Prerequisites
Requirements for the project to build and run will be added here

## Forecasted tasks:
* Modelling room
* There will be lights in the room pointing to the objects.
* Modelling a building of the museum.
* Inside the room there will be figures/objects(glut's) at every fixed distance next to the walls of the room.
* Windows for the room.
* Manipulating the objects.
* Holding objects
* Wandering the room
* Collision detection
* Modelling the door to the museum
* - Modelling external environment(nice garden)
* - Modelling the sun
* - There will be texts on top of the objects that shortly describe them.
* - At the center of the room we may have objects also.
* - The environment and each object will be illuminated taking into account all the right sources
reaching it.

## Objects in the museum

1. Skull:
http://people.sc.fsu.edu/~jburkardt/data/ply/skull.png
http://people.sc.fsu.edu/~jburkardt/data/ply/skull.ply

2. Walkman:
http://people.sc.fsu.edu/~jburkardt/data/ply/walkman.png
http://people.sc.fsu.edu/~jburkardt/data/ply/walkman.ply

5. Lamp:

https://github.com/skuhl/opengl-examples/blob/master/models/FREE-MODELS.txt

## The links to the report and the presentation
Report: https://docs.google.com/a/ku.edu.tr/document/d/1eI5gN3N_0qzh5n-oWLVaPQohBeBDHjUSEEcnhIJghOI/edit?usp=sharing

Presentation: https://docs.google.com/a/ku.edu.tr/presentation/d/1M2Fya3p4xgdAkpSQU12BUDb3gPw3q-36wW7f5V5hPQA/edit?usp=sharing