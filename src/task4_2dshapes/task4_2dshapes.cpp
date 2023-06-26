// Task 5 - Draw a 2D shape using polygons and animate it
//        - Encapsulate the shape using a class
//        - Use color_vert/frag shader to display the polygon
//        - Animate the object position, rotation and scale.

#include <iostream>
#include <vector>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <ppgso/ppgso.h>

#include <shaders/color_vert_glsl.h>
#include <shaders/color_frag_glsl.h>

const unsigned int SIZE = 512;

// Object to represent 2D OpenGL shape
class Shape {
private:
  // 2D vectors define points/vertices of the shape
  // TODO: Define your shape points
  std::vector<glm::vec3> vertices = {
          {-.5, .5, 0},
          {.5, .5, 0},
          {.5, .0, 0},
          {-.5, 0, 0},
          //head
          {.8, .25, 0},
          //leg1
          {-.6, .05, 0},
          {-.4, -.05, 0},
          {-.51, -.53, 0},
          //leg2
          {-.4, -.05, 0},
          {-.2, .05, 0},
          {-.31, -.53, 0},
          //leg3
          {.4, -.05, 0},
          {.2, .05, 0},
          {.31, -.53, 0},
          //leg4
          {.6, .05, 0},
          {.4, -.05, 0},
          {.51, -.53, 0},
          //tail
          {-.6, .8, 0},
          {-.7, .45, 0},
          //ear1
          {.30, .5, 0},
          {.40, .5, 0},
          {.35, .6, 0},
          //ear2
          {.40, .5, 0},
          {.5, .5, 0},
          {.45, .6, 0}
  };

  // Structure representing a triangular face, usually indexes into vertices
  struct Face {
    // TODO: Define your face structure
    int a;
    int b;
    int c;
  };

  // Indices define triangles that index into vertices
  // TODO: Define your mesh indices
  std::vector<Face> mesh = {
          //bpdy
          {0,  1,  2},
          {2,  3,  0},
          //head
          {1,  4,  2},
          //leg1
          {5,  6,  7},
          //leg2
          {8,  9,  10},
          //leg3
          {11, 12, 13},
          //leg4
          {14, 15, 16},
          //tail
          {17,18, 0},
          //ear1
          {19,20, 21},
          //ear2
          {22, 23, 24}
  };

  // Program to associate with the object
  ppgso::Shader program = {color_vert_glsl, color_frag_glsl};

  // These will hold the data and object buffers
  GLuint vao, vbo, cbo, ibo;
  glm::mat4 modelMatrix{1.0f};
public:
  // Public attributes that define position, color ..
  glm::vec3 position{0,0,0};
  glm::vec3 rotation{0,0,0};
  glm::vec3 scale{1,1,1};
  glm::vec3 color{1,0,0};

  // Initialize object data buffers
  Shape() {
    // Copy data to OpenGL
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Copy positions to gpu
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    // Set vertex program inputs
    auto position_attrib = program.getAttribLocation("Position");
    glEnableVertexAttribArray(position_attrib);
    glVertexAttribPointer(position_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Copy mesh indices to gpu
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.size() * sizeof(Face), mesh.data(), GL_STATIC_DRAW);

    // Set projection matrices to identity
    program.setUniform("ProjectionMatrix", glm::mat4{1.0f});
    program.setUniform("ViewMatrix", glm::mat4{1.0f});
  };
  // Clean up
  ~Shape() {
    // Delete data from OpenGL
    glDeleteBuffers(1, &ibo);
    glDeleteBuffers(1, &cbo);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
  }

  // Set the object transformation matrix
  void update() {
    // TODO: Compute transformation by scaling, rotating and then translating the shape
    modelMatrix = glm::scale(glm::mat4(), scale)
                + glm::translate(glm::mat4(), position)
                + glm::rotate(glm::mat4(), rotation.z, {0, 0, 1});
  }

  // Draw polygons
  void render(){
    // Update transformation and color uniforms in the shader
    program.use();
    program.setUniform("OverallColor", color);
    program.setUniform("ModelMatrix", modelMatrix);

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, (GLsizei) mesh.size() * 3, GL_UNSIGNED_INT, 0);
  };
};

class ShapeWindow : public ppgso::Window {
private:
  Shape shape1, shape2, shape3;
public:
  ShapeWindow() : Window{"task4_2dshapes", SIZE, SIZE} {
    shape1.color = {1,0,0};
    shape2.color = {0,1,0};
    shape3.color = {1,1,0};
  }

  void onIdle() {
    // Set gray background
    glClearColor(.1f,.1f,.1f,1.0f);
    // Clear depth and color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Move and Render shape\    // Get time for animation
    auto t = (float) glfwGetTime();

    const int moveX = 2;
    const int moveY = 1;
    // TODO: manipuate shape1 and shape2 position to rotate clockwise
    shape1.position = {t/2 - moveX,abs(sin(t * 1.6)) + moveY, 1};
    shape2.position = {t/3 - moveX,abs(sin(t + 1)) + moveY, 1};
    shape3.position = {sin(t), cos(t), 1};

    // Manipulate rotation of the shape
    //shape1.rotation.z = 1.0f;
    //shape2.rotation.z = 1.0f;
    shape3.rotation.z = t * -1.0f;

    // Manipulate shape size
    shape1.scale = {-1, -1, 1};
    shape2.scale = {-1, -1, 1};
    shape3.scale = {-1, -1, 1};

    // Update and render each shape
    shape1.update();
    shape2.update();
    shape3.update();

    shape1.render();
    shape2.render();
    shape3.render();
  }
};

int main() {
  // Create our window
  auto window = ShapeWindow{};

  // Main execution loop
  while (window.pollEvents()) {}

  return EXIT_SUCCESS;
}
