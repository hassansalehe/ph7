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
// Implements the basic functionalities of an object.
// Each object performs the following operations.
// 1. Initialization - initializing the object vertices, colors,
//    textures, and other attributes
// 2. Sending this information to the GPU
// 3. Displaing the object
// 4. Manipulating the object.
// 5. Its model-view and transformation matrices will be calculated
//    using the parent's matrices.
//
///////////////////////////////////////////////////////////////////////

#ifndef OBJECT_CLASS
#define OBJECT_CLASS

#include <vector>
#include "Angel.h"

typedef vec4  color4;
typedef vec4  point4;
typedef vec3  normal3;

class Object {
protected:

  // for holding the children of the object
  std::vector<Object *> children;
public:
  GLuint object_id;
  GLuint objectID;
  int autoOnOff = 0;

  // Model-view and projection matrices uniform location
  GLuint  ModelView, Projection;

  // for reflection and shading
  GLuint ReflectionModel;
  GLuint HS_reflection_model = 300;

  GLuint ShadingModel;
  GLuint HS_shading_model    = 274;

  // model view matrices
  mat4 model_view        = Identity();
  mat4 my_model_view     = Identity();
  mat4 parent_model_view = Identity();

  GLuint vao;
  GLuint buffer;

  size_t points_size;
  size_t colors_size;
  size_t normals_size;

  int numVertices;
  point4 * points   =  NULL;
  color4 * colors   =  NULL;
  normal3 * normals =  NULL;

  GLfloat shininess =  0.0;

  int vertexIndex;
  float scaleFactor =  1.0;

  // Array of rotation angles (in degrees) for each coordinate axis
  enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
  int  Axis = Yaxis;
  GLfloat  Theta[NumAxes]    = { 5.0, 0.0, 0.0 };
  GLfloat  Distance[NumAxes] = { 0.0, 0.0, 0.0 };

  color4 blue   = color4( 0.0, 0.0, 1.0, 1.0 );  // blue
  color4 black  = color4( 0.0, 0.0, 0.0, 1.0 );  // black
  color4 red    = color4( 1.0, 0.0, 0.0, 1.0 );  // red
  color4 yellow = color4( 1.0, 1.0, 0.0, 1.0 );  // yellow
  color4 green  = color4( 0.0, 1.0, 0.0, 1.0 );  // green
  color4 earth  = color4(125.0/255, 68.0/255, 29.0/255, 1.0 );

  // fields for picking
  int isPicking;
  color4 pickingColor;

  // for projection opetions
  enum HSprojectionOption {HS_PERSPECTIVE, HS_ORTHOGRAPHIC};
  HSprojectionOption projectionOption = HS_PERSPECTIVE;

  void initializeDataBuffers( GLuint program ) {

    // Create a vertex array object
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    points_size = sizeof(point4) * numVertices;
    colors_size = sizeof(color4) * numVertices;
    normals_size = sizeof(normal3) * numVertices;

    // Create and initialize a buffer object
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER,
                  points_size + colors_size + normals_size,
                  NULL,
                  GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, points_size, points );
    glBufferSubData( GL_ARRAY_BUFFER, points_size, colors_size, colors );
    glBufferSubData( GL_ARRAY_BUFFER,
                     points_size + colors_size,
                     normals_size,
                     normals );

    // set up vertex arrays
    size_t size = 0;
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE,
        0, BUFFER_OFFSET(size) );

    size += points_size;
    GLuint vColor = glGetAttribLocation( program, "vColor" );
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE,
        0, BUFFER_OFFSET(size) );

    size += colors_size;
    GLuint vNormal = glGetAttribLocation( program, "vNormal" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE,
        0, BUFFER_OFFSET(size) );

    glUniform1f(glGetUniformLocation(program, "Shininess"), shininess);

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

public:

  /**
   * appends a child object to this node
   */
  void appendChild(Object * child) {
    children.push_back( child );
  }


  /**
   * pushes all child nodes to a queue
   */
  void pushChildrenToQueue(queue<Object*> & queue) {

    for (Object *childObject : children) {
      queue.push( childObject );
    }
  }


  /**
   * Sends the model view matrix of this object to
   * to all its child nodes in the object tree
   */
  void sendModeViewToChildren() {
    for (Object *childObject : children) {
      childObject->getParentModelView( model_view );
    }
  }


  /**
   * A function to receive parent matrix
   */
  void getParentModelView(mat4 p_model_view ) {
    parent_model_view = p_model_view;
  }


  /**
   * Implement this function in an object for
   * for custom transformations of individual
   * objects. See the wrong rotation of skull
   */
  virtual void calculateModelViewMatrix()  = 0;
  //virtual void move();


  void changeShading() {
    HS_shading_model = (HS_shading_model == 273 ? 274: 273);
    if (HS_shading_model == 273) {
      printf("Phong\n");
    } else {
        printf("Gouraud\n");
    }

    // send to the shaders(GPU)
    glUniform1i(ShadingModel, HS_shading_model);
    glutPostRedisplay();
  }


  void changeReflection() {
    HS_reflection_model = HS_reflection_model == 300? 301 : 300;
    if (HS_reflection_model == 300) {
      printf("modified Phong refl\n");
    } else {
        printf("Phong refl\n");
    }
    glUniform1i(ReflectionModel, HS_reflection_model);
    glutPostRedisplay();
  }


  /**
   * For initializing the vertices of an object
   * and sending them to the GPU
   */
  virtual void initialize(GLuint program) = 0;


  /**
   * The general display function launched by the Glut
   */
  virtual void display( GLuint program ) {
    // bind vertex array
    glBindVertexArray( vao );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );

    // calculate model view here use this to calculate
    // custom transformations
    calculateModelViewMatrix();

    // Send values for picking
      GLuint picking = glGetUniformLocation(program, "PickingEnabled");
      glUniform1i( picking, isPicking );
      GLuint uniColor = glGetUniformLocation(program, "pickingColor");
      glUniform4fv( uniColor, 1, pickingColor );

    // send object id
    objectID = glGetUniformLocation( program, "ObjectID" );
    glUniform1i(objectID, object_id);

    // send the shading model (phong vs gouraud)
    ShadingModel = glGetUniformLocation( program, "HS_shading_model" );
    glUniform1i( ShadingModel, HS_shading_model );

    // send current reflection model
    ReflectionModel = glGetUniformLocation(program, "HS_reflection_model");
    glUniform1i( ReflectionModel, HS_reflection_model );

    // send model view
    glUniformMatrix4fv( ModelView, 1, GL_TRUE, model_view );
    glDrawArrays( GL_TRIANGLES, 0, numVertices );

    // reverse the object identifier
    glUniform1i(objectID, 0);

    // release vertex handler
    glBindVertexArray( 0 );
  }


  /**
   * The reshape function.
   */
  void reshape( int w, int h ) {
    glViewport( 0, 0, w, h );

    mat4  projection;

    switch (projectionOption)
    {
       case HS_ORTHOGRAPHIC:

         if (w <= h) {
           projection = Ortho(-1.0, 1.0,
                              -1.0 * (GLfloat) h / (GLfloat) w,
                              1.0 * (GLfloat) h / (GLfloat) w,
                              -1.0, 1.0 );
         } else {
           projection = Ortho(-1.0* (GLfloat) w / (GLfloat) h, 1.0 *
                              (GLfloat) w / (GLfloat) h,
                              -1.0, 1.0, -1.0, 1.0);
         }
         break;
       case HS_PERSPECTIVE:
          GLfloat aspect = GLfloat(w)/h;
          projection = Perspective( 45.0, aspect, 1.0, -1.0 );
          break;
    }

    glUniformMatrix4fv( Projection, 1, GL_TRUE, projection );
  }


  /**
   * the idle function
   */
  virtual void idle() = 0;

  virtual void rotateLeft( GLfloat delta ) = 0;

  virtual void rotateUp(  GLfloat delta ) = 0;

  virtual void zoomOut(GLfloat delta) {
    scaleFactor += delta;
  }


  /**
   * For zooming in the object
   */
  virtual void zoomIn(GLfloat delta) {
    scaleFactor -= delta;
  }


  /**
   * For moving toward the museum if delta > 0
   * and moving away if delta < 0
   */
  void moveForward(  GLfloat delta ) {
    printf("moving room\n");
    Distance[Zaxis] += delta;
    scaleFactor += delta;

    glutPostRedisplay();
  }

  void toggleAuto() {
    if (autoOnOff == 0) {
      autoOnOff = 1;
    } else {
      autoOnOff = 0;
    }
  }


  /**
   * Resets all parameters to defult.
   */
  virtual void reset() {
    scaleFactor = 1.0;

    Theta[Xaxis] = 5.0; Theta[Yaxis] = Theta[Zaxis] = 0.0;
    Distance[Xaxis] = Distance[Yaxis] = Distance[Zaxis] = 0.0;
  }

  void calculateNormals() {

    int triangles = numVertices / 3;
    for (int i = 0; i < triangles; i++) {

      point4 p0 = points[3*i + 0];
      point4 p1 = points[3*i + 1];
      point4 p2 = points[3*i + 2];

      normal3 n = normalize( cross((p1-p0), (p2-p0)) );

      normals[3*i + 0] = n;
      normals[3*i + 1] = n;
      normals[3*i + 2] = n;
    }

    // find the average normal of each normal of a point
    for (int i = 0; i < numVertices; i++) {

      vec3 p = vec3(0.0, 0.0, 0.0);
      int count = 0;

      for (int j = 0; j < numVertices; j++) {
        if (points[i] == points[j]) {
          p += normals[j];
          count++;
        }
      }

      // normalize the sum of neighbor normals
      //normals[i] = normalize( p );
    }
  }

  /**
   * A help function do describe each object.
   */
  virtual void help() {
  }

  virtual void checkIfPicked( unsigned char pixel[4] ) = 0;

  void enablePicking() {
    isPicking = true;
  }

  void disablePicking() {
    isPicking = false;
  }

  /**
   * For toggling projection
   */
  void changeProjection() {
    if (projectionOption == HS_PERSPECTIVE) {
       projectionOption = HS_ORTHOGRAPHIC;
    } else {
       projectionOption = HS_PERSPECTIVE;
    }
  }

  /**
   * The identity function in mat.h gives warnings,
   * decided to implement custom one.
   */
  inline mat4 Identity()
  {
    mat4 c;
    for (int i=0; i<4; i++) {
      for (int j=0; j<4; j++) {
        c[i][j]=0.0;
      }
    }
    for (int i=0; i<4; i++) {
      c[i][i] = 1.0;
    }
    return c;
  }

  /**
   * The destructor to claim memory from dynamically allocated memory
   */
  virtual ~Object() {
    if (points) delete points;
    if (colors) delete colors;
    if (normals) delete normals;
  }

};

#endif // end object class
