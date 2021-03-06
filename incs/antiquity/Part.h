////////////////////////////////////////////////////////////////////////
//
//                   COMP 510, Computer Graphics, Spring 2016
//                              Final project
//                PH7: A virtual Museum Based on OpenGL and Glut
//
//                            (c) 2016,2017,2018, - Hassan & Pirah.
//            Copying without the authors consent is strictly prohibited.
//
////////////////////////////////////////////////////////////////////////
//
// Implements the wheel object
//
// read the vertices
// read the colors
// read the face indices
// put the vertices in vertex array
// put the colors in color array
//
////////////////////////////////////////////////////////////////////////

// Scale the vertices
// send them to the GPU
#ifndef PART_CLASS
#define PART_CLASS

#include "Object.h"
#include "PLyParser.h"

class Part: public Object {

  private:

    /**
     * Reads vertices from part.ply file
     */
    void readVertices() {
      Vindex = 0;
      long nvertices, ntriangles;
      p_ply ply = ply_open(
          "incs/antiquity/objects/part.ply", NULL, 0, NULL);
      if (!ply) return; // cant open
      if (!ply_read_header(ply)) return; // cant open
      nvertices =
      ply_set_read_cb(ply, "vertex", "x", vertex_cb, NULL, X);
      ply_set_read_cb(ply, "vertex", "y", vertex_cb, NULL, Y);
      ply_set_read_cb(ply, "vertex", "z", vertex_cb, NULL, Z);

      ntriangles = ply_set_read_cb(ply,
          "face", "vertex_indices", face_cb, NULL, 0);
      printf("%ld\n%ld\n", nvertices, ntriangles);

      numVertices = ntriangles * 3; // (3 vertices/triangle)
      points      = new point4[numVertices];
      colors      = new color4[numVertices];
      normals     = new normal3[numVertices];

      r_points      = points;
      r_colors      = colors;
      r_vertexIndex = &vertexIndex;
      vertexIndex   = 0;
      c_points      = new point4[nvertices];

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

      const vec3 displacement(mid_x, mid_y,  mid_z);
      float scaleF = 0.0004 ; // manually calculated
      for (int i = 0; i < numVertices; i++) {
        points[i] = Translate(0.35, 0.0, 0.07)
                    * RotateZ(60.0)
                    * Scale(scaleF, scaleF, scaleF)
                    * Translate(-displacement)
                    * points[i];
      }

      // internal part of the part
      for (int i = 0; i < 500; i++) {// inner part
        colors[i] = color4( 0.0, 1.0, 1.0, 1.0 ); // gray
      }
      for (int i = 500; i < 2000; i++) {// inner part
        //khaki 	#F0E68C 	rgb(240,230,140)
        colors[i] = color4( 1.0, 0.0, 1.0, 1.0 );
      }
      for (int i = 2000; i < 2500; i++) {// thin metal handle
         //	Orange-Brown 	#F0F8FF 	rgb(240,248,255)
        colors[i] = color4( 0.0, 1.0, 1.0, 1.0 );
      }
      for (int i = 2500; i < 3000; i++) {// inner thin metal handle
        colors[i] = color4( 1.0, 0.0, 1.0, 1.0 );
      }
      delete c_points;
    }

  public:

    /**
     * Initializes the object data and sends to GPU
     */
    void initialize(GLuint program) {
      readVertices();
      calculateNormals();   // normals
      object_id    = 350;   // Object identifier
      isPicking    = false; // set picking color
      pickingColor = color4(0.0, 0.0, 0.2, 1.0); // (0,0,51)

      // Create a vertex array object
      glGenVertexArrays( 1, &vao );
      glBindVertexArray( vao );

      points_size = sizeof(point4)*numVertices;
      colors_size = sizeof(color4)*numVertices;

      // Create and initialize a buffer object
      glGenBuffers( 1, &buffer );
      glBindBuffer( GL_ARRAY_BUFFER, buffer );
      glBufferData( GL_ARRAY_BUFFER,
                    points_size + colors_size, NULL,
                    GL_STATIC_DRAW );
      glBufferSubData( GL_ARRAY_BUFFER, 0, points_size, points );
      glBufferSubData(GL_ARRAY_BUFFER, points_size, colors_size, colors);

      // set up vertex arrays
      GLuint vPosition = glGetAttribLocation( program, "vPosition" );
      glEnableVertexAttribArray( vPosition );
      glVertexAttribPointer( vPosition, 4, GL_FLOAT,
          GL_FALSE, 0, BUFFER_OFFSET(0) );

      GLuint vColor = glGetAttribLocation( program, "vColor" );
      glEnableVertexAttribArray( vColor );
      glVertexAttribPointer( vColor, 4, GL_FLOAT,
          GL_FALSE, 0, BUFFER_OFFSET(points_size) );

      // Set current program object
      glUseProgram( program );

      // Retrieve transformation uniform variable locations
      ModelView  = glGetUniformLocation( program, "ModelView" );
      Projection = glGetUniformLocation( program, "Projection" );

      // Set projection matrix
      mat4  projection = Ortho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
      //projection = Perspective( 45.0, 1.0, 0.5, 3.0 );
      glUniformMatrix4fv( Projection, 1, GL_TRUE, projection );
      glEnable( GL_DEPTH_TEST ); // Enable hiddden surface removal
      glClearColor( 1.0, 1.0, 1.0, 1.0 ); // clear buffer
    }

    void calculateModelViewMatrix() {
       model_view = parent_model_view * my_model_view;
    }

    void idle( void ) {
      if (autoOnOff != 0){
        my_model_view = my_model_view
                        * Translate(0.35, 0.0, 0.07)
                        * RotateY(0.5)
                        * Translate(-0.35, 0.0, -0.07);
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
      if ( pixel[0] == 0 && pixel[1] == 0 && pixel[2] == 51 ) { // part
#ifdef DEBUG
        printf("Part selected\n");
#endif
		    my_model_view = my_model_view
                        * Translate(0.35, 0.0, 0.07)
                        * RotateY(30)
                        * Translate(-0.35, 0.0, -0.07);
      }
    }
};

#endif // end part
