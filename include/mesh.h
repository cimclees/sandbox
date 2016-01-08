#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <GL/glew.h>
#include "../lib/obj_loader.h"

class Vertex {
  public:
    Vertex(const glm::vec3& pos, const glm::vec2& texCoord, 
           const glm::vec3& normal = glm::vec3(0,0,0)) {
      this->pos = pos;
      this->texCoord = texCoord;
      this->normal = normal;
    }

    inline glm::vec3* GetPos() { return &pos; }
    inline glm::vec2* GetTexCoord() { return &texCoord; }
    inline glm::vec3* GetNormal() { return & normal; }
  protected:
  private:
    glm::vec3 pos;      // Position of vertex
    glm::vec2 texCoord; // Texture coordinate (pixel) to map to vertex
    glm::vec3 normal;   // Up direction on a surface
};

class Mesh {
  public:
    Mesh(Vertex* vertices, unsigned int numVertices, unsigned int* indices,
         unsigned int numIndices);

    Mesh(const std::string& fileName);

    void Draw();

    virtual ~Mesh();
  protected:
  private:
    Mesh(const Mesh& other);
    void operator=(const Mesh& other);

    void InitMesh(const IndexedModel& model);

    enum {
      POSITION_VB,
      TEXCOORD_VB,
      NORMAL_VB,

      INDEX_VB,

      NUM_BUFFERS
    };

    GLuint m_vertexArrayObject;
    GLuint m_vertexArrayBuffers[NUM_BUFFERS];
    unsigned int m_drawCount;
};

#endif // MESH_H