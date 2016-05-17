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
// Implements the room of the museum
//
///////////////////////////////////////////////////////////////////////////////

#ifndef ROOF_CLASS
#define ROOF_CLASS

#include "Object.h"

typedef vec4  color4;
typedef vec4  point4;

class Roof: public Object {
  private:
    // Vertices of a unit cube centered at origin, sides aligned with axes
    point4 vertices[6] = {
      point4( -0.6,  0.45,  0.6, 1.0 ),
      point4(  0.0,  0.8,  0.6, 1.0 ),
      point4(  0.0,  0.8, -0.6, 1.0 ),
      point4( -0.6,  0.45, -0.6, 1.0 ),
      point4(  0.6,  0.45,  0.6, 1.0 ),
      point4(  0.6,  0.45, -0.6, 1.0 ),
    };

    void quad( int a, int b, int c, int d ) {
      // Initialize colors
      colors[vertexIndex] = blue; points[vertexIndex] = vertices[a]; vertexIndex++;
      colors[vertexIndex] = blue; points[vertexIndex] = vertices[b]; vertexIndex++;
      colors[vertexIndex] = blue; points[vertexIndex] = vertices[c]; vertexIndex++;
      colors[vertexIndex] = blue; points[vertexIndex] = vertices[a]; vertexIndex++;
      colors[vertexIndex] = blue; points[vertexIndex] = vertices[c]; vertexIndex++;
      colors[vertexIndex] = blue; points[vertexIndex] = vertices[d]; vertexIndex++;
    }

    // generate 12 triangles: 36 vertices and 36 colors
    void colorcube() {
      quad( 0, 1, 2, 3 ); // left roof
      quad( 1, 4, 5, 2 ); // right roof
    }

  public:
    /**
     * Initializes the vertices and colors of the empty room object.
     */
    void initialize(GLuint program) {
      numVertices = 12; //(2 faces)(2 triangles/face)(3 vertices/triangle)
      points = new point4[numVertices];
      colors = new color4[numVertices];

      // quad generates two triangles for each face and assigns colors
      //    to the vertices
      vertexIndex = 0;
      colorcube();

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
      //mat4 projection = Perspective( 45.0, 1.0, 0.5, 3.0 );
      glUniformMatrix4fv( Projection, 1, GL_TRUE, projection );

      // Enable hiddden surface removal
      glEnable( GL_DEPTH_TEST );

      // Set state variable "clear color" to clear buffer with.
      glClearColor( 1.0, 1.0, 1.0, 1.0 );
    }

    void display( GLuint program )
    {
      glBindVertexArray( vao );
      glBindBuffer( GL_ARRAY_BUFFER, buffer );

      // set up vertex arrays
      //GLuint vPosition = glGetAttribLocation( program, "vPosition" );
      //glEnableVertexAttribArray( vPosition );
      //glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

      //GLuint vColor = glGetAttribLocation( program, "vColor" );
      //glEnableVertexAttribArray( vColor );
     // glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(points_size) );

      //  Generate tha model-view matrix
      mat4 scale = Scale( scaleFactor, scaleFactor, scaleFactor );
      const vec3 displacement( 0.0, 0.0, 0.0 );
      mat4  model_view = ( scale * Translate( displacement ) *
              RotateX( Theta[Xaxis] ) *
              RotateY( Theta[Yaxis] ) // *
             // RotateZ( Theta[Zaxis] )
                         );
      /* // For perspective projection
      vec3 viewer_pos = vec3( 0.0, 0.0, 2.45 );
      model_view = ( Translate( -viewer_pos ) * scale * Translate( displacement ) *
              RotateX( Theta[Xaxis] ) *
              RotateY( Theta[Yaxis] ) // *
             // RotateZ( Theta[Zaxis] )
                         );
      */
      glUniformMatrix4fv( ModelView, 1, GL_TRUE, model_view );
      glDrawArrays( GL_TRIANGLES, 0, numVertices );

      glBindVertexArray( 0 );
      //glDisableVertexAttribArray(vPosition);
      //glDisableVertexAttribArray(vColor);
    }

    void idle( void )
    {
      //Theta[Axis] += 0.1;

      //if ( Theta[Axis] > 360.0 ) {
      //    Theta[Axis] -= 360.0;
      //}

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

    ~Roof() {
      delete colors;
      delete points;
    }
};

#endif // end room
