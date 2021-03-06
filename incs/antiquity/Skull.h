////////////////////////////////////////////////////////////////////////
//
//                   COMP 510, Computer Graphics, Spring 2016
//                              Final project
//                PH7: A virtual Museum Based on OpenGL and Glut
//
//                            (c) 2016,2017,2018 - Hassan & Pirah.
//            Copying without the authors consent is strictly prohibited.
//
////////////////////////////////////////////////////////////////////////
//
// read the vertices
// read the colors
// read the face indices
// put the vertices in vertex array
// put the colors in color array
//
////////////////////////////////////////////////////////////////////////

// Scale the vertices send them to the GPU
#ifndef HUMAN_SKULL_CLASS
#define HUMAN_SKULL_CLASS

#include "Object.h"
#include "PLyParser.h"

class Skull: public Object {

  vec3 displacement_for_rotation;

  public:
  int object_id = 300;

  private:
    void readVertices() {

      Vindex = 0;
      long nvertices, ntriangles;
      p_ply ply = ply_open(
          "incs/antiquity/objects/skull.ply", NULL, 0, NULL);
      if (!ply) return; // cant open
      if (!ply_read_header(ply)) return; // cant open
      nvertices =
      ply_set_read_cb(ply, "vertex", "x", vertex_cb, NULL, X);
      ply_set_read_cb(ply, "vertex", "y", vertex_cb, NULL, Y);
      ply_set_read_cb(ply, "vertex", "z", vertex_cb, NULL, Z);

      ply_set_read_cb(ply, "vertex", "red", color_cb, NULL, RED);
      ply_set_read_cb(ply, "vertex", "green", color_cb, NULL, GREEN);
      ply_set_read_cb(ply, "vertex", "blue", color_cb, NULL, BLUE);

      ntriangles = ply_set_read_cb(ply, "face", "vertex_indices", face_cb, NULL, 0);
      // printf("%ld\n%ld\n", nvertices, ntriangles);

      numVertices = ntriangles * 3; // (3 vertices/triangle)
      points      = new point4[numVertices];
      colors      = new color4[numVertices];

      r_points      = points;
      r_colors      = colors;
      r_vertexIndex = &vertexIndex;
      vertexIndex   = 0;

      c_points = new point4[nvertices];
      c_colors = new color4[nvertices];

      if (!ply_read(ply)) return; // cant open
      ply_close(ply);

      // scale the points
      float min_x = c_points[0].x;
      float max_x = c_points[0].x;

      float min_y = c_points[0].y;
      float max_y = c_points[0].y;

      float min_z = c_points[0].z;
      float max_z = c_points[0].z;

      for (int i = 0; i < Vindex; i++) {
        min_x = MIN(min_x, c_points[i].x);
        min_y = MIN(min_y, c_points[i].y);
        min_z = MIN(min_z, c_points[i].z);

        max_x = MAX(max_x, c_points[i].x);
        max_y = MAX(max_y, c_points[i].y);
        max_z = MAX(max_z, c_points[i].z);
      }

      // translate according to mid-point
      float mid_x = (min_x + max_x) / 2.0;
      float mid_y = (min_z + max_y) / 2.0;
      float mid_z = (min_z + max_z) / 2.0;

      const vec3 displacement(-2 * mid_x, mid_y,  mid_z);
      displacement_for_rotation=displacement;
      float scaleF = 0.00089975 ; // manually calculated
      for (int i = 0; i < vertexIndex; i++) {
        points[i] = Translate(-0.35, 0.0, 0.045)
                    * Scale(scaleF, scaleF, scaleF)
                    * Translate(-displacement)
                    * RotateY(90.0)
                    * points[i];
      }

      // reclaim memory
      delete c_colors;
      delete c_points;
    }
public:
    void initialize(GLuint program) {

      readVertices();
      isPicking = false;  // set picking color
      pickingColor = color4(0.2, 0.2, 0.0, 1.0); //(51, 51, 0)

      // Create a vertex array object
      glGenVertexArrays( 1, &vao );
      glBindVertexArray( vao );

      points_size = sizeof (point4) * numVertices;
      colors_size = sizeof (color4) * numVertices;

      // Create and initialize a buffer object
      glGenBuffers( 1, &buffer );
      glBindBuffer( GL_ARRAY_BUFFER, buffer );
      glBufferData( GL_ARRAY_BUFFER,
                    points_size + colors_size + normals_size, NULL,
                    GL_STATIC_DRAW );
      glBufferSubData( GL_ARRAY_BUFFER, 0, points_size, points );
      glBufferSubData(GL_ARRAY_BUFFER, points_size, colors_size, colors);

      // set up vertex arrays
      size_t size = 0;
      GLuint vPosition = glGetAttribLocation( program, "vPosition" );
      glEnableVertexAttribArray( vPosition );
      glVertexAttribPointer( vPosition, 4, GL_FLOAT,
          GL_FALSE, 0, BUFFER_OFFSET(size) );

      size += points_size;
      GLuint vColor = glGetAttribLocation( program, "vColor" );
      glEnableVertexAttribArray( vColor );
      glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0,
          BUFFER_OFFSET(size) );

      shininess = 120;
      glUniform1f(glGetUniformLocation(program, "Shininess"), shininess);
      glUseProgram( program ); // Set current program object

      // Retrieve transformation uniform variable locations
      ModelView  = glGetUniformLocation( program, "ModelView" );
      Projection = glGetUniformLocation( program, "Projection" );

      // Set projection matrix
      mat4  projection = Ortho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
      //projection = Perspective( 45.0, 1.0, 0.5, 3.0 );
      glUniformMatrix4fv( Projection, 1, GL_TRUE, projection );
      glEnable( GL_DEPTH_TEST ); // Enable hiddden surface removal

      // Set state variable "clear color" to clear buffer with.
      glClearColor( 1.0, 1.0, 1.0, 1.0 );
    }

    void calculateModelViewMatrix() {
      //  Generate tha model-view matrix
      const vec3 displacement(
          Distance[Xaxis], Distance[Yaxis], Distance[Zaxis] );
      model_view = parent_model_view * my_model_view;
    }

    void idle( void ) {
      if (autoOnOff != 0) {
        my_model_view = my_model_view
                        * Translate(-0.35, 0.0, 0.045)
                        * RotateY(0.5)
                        * Translate(0.35, 0.0, -0.045);
      }
      glutPostRedisplay();
    }

    void rotateLeft(float delta) {

      Theta[ Yaxis ] += delta;
      if ( Theta[ Yaxis ] > 360.0 ) {
          Theta[ Yaxis ] -= 360.0;
      }
      glutPostRedisplay();
    }


    void rotateUp(float delta) {

      Theta[ Xaxis ] += delta;
      if ( Theta[ Xaxis ] > 360.0 ) {
          Theta[ Xaxis ] -= 360.0;
      }
      glutPostRedisplay();
    }


    void checkIfPicked( unsigned char pixel[4] ) {
      if ( pixel[0] == 51 && pixel[1] == 51 && pixel[2] == 0 ) { // Skull
#ifdef DEBUG
        printf("Skull selected\n");
#endif
		    my_model_view = my_model_view
                        * Translate(-0.35, 0.0, 0.045)
                        * RotateY(30)
                        * Translate(0.35, 0.0, -0.045);
      }
    }
};

#endif // end skull
