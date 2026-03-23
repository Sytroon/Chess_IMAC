#pragma once
#include <glad/glad.h>
#include "glimac/glm.hpp"
#include <vector>

struct Vertex3D {
    glm::vec3 position;
    glm::vec3 normal;

    Vertex3D(glm::vec3 p, glm::vec3 n) : position(p), normal(n) {}
};

class Renderer3D {
private:
    GLuint m_vbo, m_vao, m_ibo;
    GLsizei m_indexCount;
    
public:
    Renderer3D() {
        // 1. Définition des sommets d'un cube (Position, Normale)
        // On définit 24 sommets (6 faces * 4 sommets) pour avoir des normales propres par face
        std::vector<Vertex3D> vertices = {
            // Face avant (Normal Z+)
            {{-0.5,-0.5, 0.5}, {0,0,1}}, {{0.5,-0.5, 0.5}, {0,0,1}}, {{0.5, 0.5, 0.5}, {0,0,1}}, {{-0.5, 0.5, 0.5}, {0,0,1}},
            // Face arrière (Normal Z-)
            {{-0.5,-0.5,-0.5}, {0,0,-1}}, {{0.5,-0.5,-0.5}, {0,0,-1}}, {{0.5, 0.5,-0.5}, {0,0,-1}}, {{-0.5, 0.5,-0.5}, {0,0,-1}},
            // Face haut (Normal Y+)
            {{-0.5, 0.5,-0.5}, {0,1,0}}, {{0.5, 0.5,-0.5}, {0,1,0}}, {{0.5, 0.5, 0.5}, {0,1,0}}, {{-0.5, 0.5, 0.5}, {0,1,0}},
            // Face bas (Normal Y-)
            {{-0.5,-0.5,-0.5}, {0,-1,0}}, {{0.5,-0.5,-0.5}, {0,-1,0}}, {{0.5,-0.5, 0.5}, {0,-1,0}}, {{-0.5,-0.5, 0.5}, {0,-1,0}},
            // Face droite (Normal X+)
            {{0.5,-0.5,-0.5}, {1,0,0}}, {{0.5, 0.5,-0.5}, {1,0,0}}, {{0.5, 0.5, 0.5}, {1,0,0}}, {{0.5,-0.5, 0.5}, {1,0,0}},
            // Face gauche (Normal X-)
            {{-0.5,-0.5,-0.5}, {-1,0,0}}, {{-0.5, 0.5,-0.5}, {-1,0,0}}, {{-0.5, 0.5, 0.5}, {-1,0,0}}, {{-0.5,-0.5, 0.5}, {-1,0,0}},
        };

        uint32_t indices[] = {
            0, 1, 2, 0, 2, 3,       // Face avant
            4, 5, 6, 4, 6, 7,       // Face arrière
            8, 9, 10, 8, 10, 11,    // Face haut
            12, 13, 14, 12, 14, 15, // Face bas
            16, 17, 18, 16, 18, 19, // Face droite
            20, 21, 22, 20, 22, 23  // Face gauche
        };

        m_indexCount = 36; // 6 faces * 6 indices par face

        // 2. Création des buffers (VBO, IBO, VAO) comme dans ton TD
        glGenBuffers(1, &m_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex3D), vertices.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &m_ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
        
        glEnableVertexAttribArray(0); // Position
        glEnableVertexAttribArray(1); // Normale
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (const GLvoid*)0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (const GLvoid*)offsetof(Vertex3D, normal));
        
        glBindVertexArray(0);
    }

    void drawCube() {
        glBindVertexArray(m_vao);
        glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
};