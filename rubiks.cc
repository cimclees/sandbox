/**
 * @file rubiks.cc
 * @author Charles Ian Mclees
 *  
 * @section DESCRIPTION
 *
 * This file contains the main function for a Rubik's Cube game.
 */

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include "display.h"
#include "mesh.h"
#include "shader.h"
#include "camera.h"
#include "transform.h"
#include "cube.h"

#include <iostream>

// Window size
#define WIDTH 800
#define HEIGHT 600

#define MOUSE_SENS 0.005f

/**
 * Function to position and orient a camera.
 *
 * @param position
 * @param forward
 * @param horizOffset
 * @param vertOffset
 */
void positionCam(glm::vec3& position, glm::vec3& forward,
                 const float& horizOffset, const float& vertOffset) {
    float sinHoriz = sinf(horizOffset);
    float cosHoriz = cosf(horizOffset);
    position.x = 20 * sinHoriz;
    position.z = 20 * cosHoriz;
    forward.x = -sinHoriz;
    forward.z = -cosHoriz;
    
    float sinVert = sinf(vertOffset);
    position.y = 20 * sinVert;
    forward.y = -sinVert;
}

/**
 * Main function to run a Rubik's Cube game.
 */
int main() {
  // Open a window.
  Display display(WIDTH, HEIGHT, "Rubik's Cube");
  // Load 3D data for a block model.
  Mesh blockMesh("./res/block.obj");
  //
  Shader shader("./res/basicShader");
  // Create a camera object to manipulate positional perspective.
  float horizOffset = 0.0f;
  float vertOffset  = 0.0f;
  Camera camera(glm::vec3(0,0,0), 70.0f, ((float) WIDTH) / HEIGHT, 
                0.01f, 1000.0f);

  // Create a transform object to perform rotational and positional transforms
  // on block objects.
  Transform transform;
  // Create a cube object of size 3x3x3.
  Cube cube(3);

  cube.SetRandRotation();
  
  bool quit = false;
  bool rightClick = false;
  // Iterate over drawn frames.
  while (!display.IsClosed()) {
    display.Clear(0.0f, 0.15f, 0.3f, 1.0f);
    
    // Change camera position and orientation.
    positionCam(camera.GetPos(), camera.GetFor(), horizOffset, vertOffset);

    // Continue any current cube animations.
    cube.UpdateRotation();

    cube.SetRandRotation();
    
    cube.Draw(shader, transform, camera, blockMesh);

    // Process user input.
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) { 
        case SDL_QUIT: {
          quit = true;
          break;
        }
        case SDL_MOUSEMOTION: {
          if (rightClick) {
            horizOffset += (event.motion.xrel * -MOUSE_SENS);
            if ((event.motion.yrel < 0 && sinf(vertOffset) > (-PI / 4.0f)) ||
                (event.motion.yrel > 0 && sinf(vertOffset) < (PI / 4.0f))) {
              vertOffset  += (event.motion.yrel * MOUSE_SENS);
            }
          }
          break;
        }
        case SDL_MOUSEBUTTONDOWN: {
          if (event.button.button == SDL_BUTTON_RIGHT) {
            rightClick = true;
          } else if (event.button.button == SDL_BUTTON_LEFT) {
            
            glm::vec3 rayStart;
            glm::vec3 rayEnd;

            camera.GetPickRay(event.button.x, event.button.y, rayStart, rayEnd);
            
            // Ray Collision Check
            glm::vec3 currPoint(rayStart);
            bool found = false;
          
            glm::vec3 delta = normalize(rayEnd - rayStart) / 100.0f; 
            
            int x = 0;
            int y = 0;
            int z = 0;
            int checks = 0;

            // Get close to cube
            while (fabs(currPoint.x) > 3.1f &&
                   fabs(currPoint.y) > 3.1f &&
                   fabs(currPoint.z) > 3.1f) {
              currPoint.x += delta.x;
              currPoint.y += delta.y;
              currPoint.z += delta.z;
            }

            while (!found && (fabs(currPoint.x) < 3.2f
                              ||  fabs(currPoint.y) < 3.2f
                              ||  fabs(currPoint.z) < 3.2f)) {

              checks++;
              currPoint.x += delta.x;
              currPoint.y += delta.y;
              currPoint.z += delta.z;

              for (x = 0; x < 3 && !found; x++) {
                for (y = 0; y < 3 && !found; y++) {
                  for (z = 0; z < 3 && !found; z++) {
                      if (fabs(cube.GetPos(x, y, z).x - currPoint.x) < 1.0f
                        && fabs(cube.GetPos(x, y, z).y - currPoint.y) < 1.0f
                        && fabs(cube.GetPos(x, y, z).z - currPoint.z) < 1.0f) {
                      found = true;
                      std::cout << "found: " << checks << std::endl;
                      cube.GetSelected() = glm::vec3(x, y, z);
                    }
                  }
                }
              }  
            }

            if (!found) {
              std::cout << "not found: " << checks << std::endl;
              cube.GetSelected() = glm::vec3(-1, -1, -1);
            }
          }
          break;
        }

        case SDL_MOUSEBUTTONUP: {
          if (event.button.button == SDL_BUTTON_RIGHT) {
            rightClick = false;
          }
          break;
        }
      }
    }
    display.Update(quit);
  }
  return 0;
}
