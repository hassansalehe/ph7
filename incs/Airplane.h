

// read the vertices
// read the colors
// read the face indices
// put the vertices in vertex array
// put the colors in color array

// Scale the vertices
// send them to the GPU
#ifndef Airplane_CLASS
#define Airplane_CLASS

#include "Object.h"
#include "PLyParser.h"

class Airplane: public Object {
  mat4 my_model_view= identity();
  float max_v = 0.0;

  private:
    void readVertices() {
      Vindex = 0;
      long nvertices, ntriangles;
      p_ply ply = ply_open("incs/objects/airoplane.ply", NULL, 0, NULL);
      if (!ply) return; // cant open
      if (!ply_read_header(ply)) return; // cant open
      nvertices =
      ply_set_read_cb(ply, "vertex", "x", vertex_cb, NULL, X);
      ply_set_read_cb(ply, "vertex", "y", vertex_cb, NULL, Y);
      ply_set_read_cb(ply, "vertex", "z", vertex_cb, NULL, Z);

      ntriangles = ply_set_read_cb(ply, "face", "vertex_indices", face_cb, NULL, 0);
      printf("%ld\n%ld\n", nvertices, ntriangles);

      numVertices = ntriangles * 3; //(180 faces)(2 triangles/face)(3 vertices/triangle)
      points = new point4[numVertices];
      colors = new color4[numVertices];

      r_points = points;
      r_colors = colors;
      r_vertexIndex = &vertexIndex;

      vertexIndex = 0;

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

      for(int i = 0; i < Vindex; i++)
      {
        min_x = MIN(min_x, c_points[i].x);
        min_y = MIN(min_y, c_points[i].y);
        min_z = MIN(min_z, c_points[i].z);

        max_x = MAX(max_x, c_points[i].x);
        max_y = MAX(max_y, c_points[i].y);
        max_z = MAX(max_z, c_points[i].z);


        if(abs(c_points[i].x) > max_v )
          max_v = abs(c_points[i].x);

        if(abs(c_points[i].y) > max_v )
          max_v = abs(c_points[i].y);

        if(abs(c_points[i].z) > max_v )
          max_v = abs(c_points[i].z);

      }

      printf("min x %f, max x %f\n", min_x, max_x);
      printf("min y %f, max y %f\n", min_y, max_y);
      printf("min z %f, max z %f\n", min_z, max_z);

      // translate according to mid-point
      float mid_x = (min_x + max_x) / 2.0;
      float mid_y = (min_z + max_y) / 2.0;
      float mid_z = (min_z + max_z) / 2.0;


      //Distance[Xaxis] = -2.0;
      //Distance[Yaxis] = -0.3;
      //Distance[Zaxis] = -0.4;

      // scaleFactor = 0.2;
      //Distance[Xaxis] = 0.38;
      //Distance[Zaxis] = 0.2;


       const vec3 displacement(mid_x, mid_y,  mid_z);
       float scaleF = 0.00014 ; // manually calculated
      for(int i = 0; i < numVertices; i++)
      {
        points[i] = Translate(-0.35, -0.02, 0.42) * RotateX(120.0) * Scale(scaleF, scaleF, scaleF) * Translate(-displacement)  *   points[i];

      }

      // internal part of the Plane
      for(int i = 0; i < 500; i++) // inner part
        colors[i] = color4(1.0, 0.5, 0.0); // gray

      for(int i = 500; i < 2000; i++) // inner part
        //khaki 	#F0E68C 	rgb(240,230,140)
        colors[i] = color4(0.9, 0.8, 0.5);


      for(int i = 2000; i < 2500; i++) // thin metal handle
         //	Orange-Brown 	#F0F8FF 	rgb(240,248,255)
        colors[i] = color4(1.0, 0.5, 0.0);

      for(int i = 2500; i < 3000; i++) // inner thin metal handle
        colors[i] = color4(0.9, 0.9, 0.5);

      for(int i = 3000; i < 3500; i++)
         //	aliceblue 	#F0F8FF 	rgb(240,248,255)
        colors[i] = color4(1.0, 0.5, 0.0);

      for(int i = 3500; i < 4000; i++)
         //	aliceblue 	#F0F8FF 	rgb(240,248,255)
        colors[i] =color4(0.9, 0.8, 0.5);


      // reclaim memory
      delete c_colors;
      delete c_points;

      }
  public:
    void initialize(GLuint program) {

      readVertices(); // parse vertices from file

      // Object identifier
      object_id = 320;

      // Create a vertex array object
      glGenVertexArrays( 1, &vao );
      glBindVertexArray( vao );

      points_size = sizeof(point4)*numVertices;
      colors_size = sizeof(color4)*numVertices;

      // Create and initialize a buffer object
      glGenBuffers( 1, &buffer );
      glBindBuffer( GL_ARRAY_BUFFER, buffer );
      glBufferData( GL_ARRAY_BUFFER, points_size + colors_size, NULL, GL_STATIC_DRAW );
      glBufferSubData( GL_ARRAY_BUFFER, 0, points_size, points );
      glBufferSubData( GL_ARRAY_BUFFER, points_size, colors_size, colors );

      // set up vertex arrays
      GLuint vPosition = glGetAttribLocation( program, "vPosition" );
      glEnableVertexAttribArray( vPosition );
      glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

      GLuint vColor = glGetAttribLocation( program, "vColor" );
      glEnableVertexAttribArray( vColor );
      glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(points_size) );

      // Set current program object
      glUseProgram( program );

      // Retrieve transformation uniform variable locations
      ModelView = glGetUniformLocation( program, "ModelView" );
      Projection = glGetUniformLocation( program, "Projection" );

      // Set projection matrix
      mat4  projection = Ortho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
      //projection = Perspective( 45.0, 1.0, 0.5, 3.0 );
      glUniformMatrix4fv( Projection, 1, GL_TRUE, projection );

      // Enable hiddden surface removal
      glEnable( GL_DEPTH_TEST );

      // Set state variable "clear color" to clear buffer with.
      glClearColor( 1.0, 1.0, 1.0, 1.0 );
    }

    void calculateModelViewMatrix() {
       model_view =parent_model_view*my_model_view;
    }

    void idle( void )
    {
     my_model_view= my_model_view*Translate(-0.35, -0.02, 0.42)*RotateY(0.5)*Translate(0.35, 0.02, -0.42);

      glutPostRedisplay();
    }
    void rotateLeft(float delta) {

      Theta[Yaxis] += delta;
      if ( Theta[Yaxis] > 360.0 ) {
          Theta[Yaxis] -= 360.0;
      }
      glutPostRedisplay();
    }

    void rotateUp(float delta) {

      Theta[Xaxis] += delta;
      if ( Theta[Xaxis] > 360.0 ) {
          Theta[Xaxis] -= 360.0;
      }
      glutPostRedisplay();
    }
//     void move(){
// 	  const vec3 displacement( Distance[Xaxis], Distance[Yaxis], Distance[Zaxis] );
//       my_model_view=my_model_view*Translate(displacement*RotateY(10)*Translate(-displacement);
// 	}

    ~Airplane() {
      delete colors;
      delete points;
    }
};

#endif // end walkman