#pragma once

#include <glad/glad.h>
#include "glimac/Geometry.hpp"
#include "glimac/glm.hpp"
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

struct Vertex3D {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;

    Vertex3D(const glm::vec3& p, const glm::vec3& n, const glm::vec2& uv)
        : position(p), normal(n), texCoord(uv) {}
};

class Renderer3D {
public:
    enum class PieceMeshType {
        Pawn = 0,
        Rook,
        Knight,
        Bishop,
        Queen,
        King,
        Decoration
    };

private:
    struct MeshBuffers {
        GLuint  vbo        = 0;
        GLuint  vao        = 0;
        GLuint  ibo        = 0;
        GLsizei indexCount = 0;

        bool isValid() const { return vao != 0 && indexCount > 0; }
    };

    MeshBuffers                       m_cube;
    std::array<MeshBuffers, 7>        m_pieceMeshes{};

    static constexpr float kMinExtent = 0.0001f;

    MeshBuffers createMesh(const std::vector<Vertex3D>& vertices, const std::vector<uint32_t>& indices) {
        MeshBuffers mesh;
        if (vertices.empty() || indices.empty()) {
            return mesh;
        }

        glGenVertexArrays(1, &mesh.vao);
        glBindVertexArray(mesh.vao);

        glGenBuffers(1, &mesh.vbo);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex3D)), vertices.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &mesh.ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size() * sizeof(uint32_t)), indices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), nullptr);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), reinterpret_cast<const void*>(offsetof(Vertex3D, normal)));
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), reinterpret_cast<const void*>(offsetof(Vertex3D, texCoord)));

        glBindVertexArray(0);
        mesh.indexCount = static_cast<GLsizei>(indices.size());
        return mesh;
    }

    void destroyMesh(MeshBuffers& mesh) {
        if (mesh.ibo != 0) {
            glDeleteBuffers(1, &mesh.ibo);
        }
        if (mesh.vbo != 0) {
            glDeleteBuffers(1, &mesh.vbo);
        }
        if (mesh.vao != 0) {
            glDeleteVertexArrays(1, &mesh.vao);
        }
        mesh = {};
    }

    void drawMesh(const MeshBuffers& mesh) const {
        if (!mesh.isValid()) {
            return;
        }

        glBindVertexArray(mesh.vao);
        glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    void createCubeMesh() {
        const std::vector<Vertex3D> vertices = {
            {{-0.5f, -0.5f,  0.5f}, { 0.f,  0.f,  1.f}, {0.f, 0.f}}, {{ 0.5f, -0.5f,  0.5f}, { 0.f,  0.f,  1.f}, {1.f, 0.f}}, {{ 0.5f,  0.5f,  0.5f}, { 0.f,  0.f,  1.f}, {1.f, 1.f}}, {{-0.5f,  0.5f,  0.5f}, { 0.f,  0.f,  1.f}, {0.f, 1.f}},
            {{-0.5f, -0.5f, -0.5f}, { 0.f,  0.f, -1.f}, {1.f, 0.f}}, {{ 0.5f, -0.5f, -0.5f}, { 0.f,  0.f, -1.f}, {0.f, 0.f}}, {{ 0.5f,  0.5f, -0.5f}, { 0.f,  0.f, -1.f}, {0.f, 1.f}}, {{-0.5f,  0.5f, -0.5f}, { 0.f,  0.f, -1.f}, {1.f, 1.f}},
            {{-0.5f,  0.5f, -0.5f}, { 0.f,  1.f,  0.f}, {0.f, 0.f}}, {{ 0.5f,  0.5f, -0.5f}, { 0.f,  1.f,  0.f}, {1.f, 0.f}}, {{ 0.5f,  0.5f,  0.5f}, { 0.f,  1.f,  0.f}, {1.f, 1.f}}, {{-0.5f,  0.5f,  0.5f}, { 0.f,  1.f,  0.f}, {0.f, 1.f}},
            {{-0.5f, -0.5f, -0.5f}, { 0.f, -1.f,  0.f}, {0.f, 0.f}}, {{ 0.5f, -0.5f, -0.5f}, { 0.f, -1.f,  0.f}, {1.f, 0.f}}, {{ 0.5f, -0.5f,  0.5f}, { 0.f, -1.f,  0.f}, {1.f, 1.f}}, {{-0.5f, -0.5f,  0.5f}, { 0.f, -1.f,  0.f}, {0.f, 1.f}},
            {{ 0.5f, -0.5f, -0.5f}, { 1.f,  0.f,  0.f}, {0.f, 0.f}}, {{ 0.5f,  0.5f, -0.5f}, { 1.f,  0.f,  0.f}, {1.f, 0.f}}, {{ 0.5f,  0.5f,  0.5f}, { 1.f,  0.f,  0.f}, {1.f, 1.f}}, {{ 0.5f, -0.5f,  0.5f}, { 1.f,  0.f,  0.f}, {0.f, 1.f}},
            {{-0.5f, -0.5f, -0.5f}, {-1.f,  0.f,  0.f}, {0.f, 0.f}}, {{-0.5f,  0.5f, -0.5f}, {-1.f,  0.f,  0.f}, {1.f, 0.f}}, {{-0.5f,  0.5f,  0.5f}, {-1.f,  0.f,  0.f}, {1.f, 1.f}}, {{-0.5f, -0.5f,  0.5f}, {-1.f,  0.f,  0.f}, {0.f, 1.f}},
        };

        const std::vector<uint32_t> indices = {
            0, 1, 2, 0, 2, 3,
            4, 5, 6, 4, 6, 7,
            8, 9, 10, 8, 10, 11,
            12, 13, 14, 12, 14, 15,
            16, 17, 18, 16, 18, 19,
            20, 21, 22, 20, 22, 23
        };

        m_cube = createMesh(vertices, indices);
    }

    bool loadPieceMesh(PieceMeshType type, const std::string& objPath) {
        glimac::Geometry geometry;
        const auto modelPath = glimac::FilePath(objPath);

        if (!geometry.loadOBJ(modelPath, modelPath.dirPath(), false)) {
            std::cerr << "[WARNING] Impossible de charger le modèle 3D : " << objPath << '\n';
            return false;
        }

        const auto* sourceVertices = geometry.getVertexBuffer();

        std::vector<Vertex3D> vertices;
        vertices.reserve(geometry.getVertexCount());
        for (size_t i = 0; i < geometry.getVertexCount(); ++i) {
            const auto& vertex = sourceVertices[i];
            vertices.emplace_back(vertex.m_Position, vertex.m_Normal, vertex.m_TexCoords);
        }

        std::vector<uint32_t> indices;
        indices.reserve(geometry.getIndexCount());
        const auto* sourceIndices = geometry.getIndexBuffer();
        for (size_t i = 0; i < geometry.getIndexCount(); ++i) {
            indices.push_back(static_cast<uint32_t>(sourceIndices[i]));
        }

        auto& mesh = m_pieceMeshes[static_cast<size_t>(type)];
        destroyMesh(mesh);
        mesh = createMesh(vertices, indices);
        return mesh.isValid();
    }

    void loadPieceMeshes(const std::string& assetsRoot) {
        loadPieceMesh(PieceMeshType::Pawn, assetsRoot + "/Chess_Piece/pawn.obj");
        loadPieceMesh(PieceMeshType::Bishop, assetsRoot + "/Chess_Piece/bishop.obj");
        loadPieceMesh(PieceMeshType::King, assetsRoot + "/Chess_Piece/king.obj");
        loadPieceMesh(PieceMeshType::Knight, assetsRoot + "/Chess_Piece/knight.obj");
        loadPieceMesh(PieceMeshType::Queen, assetsRoot + "/Chess_Piece/queen.obj");
        loadPieceMesh(PieceMeshType::Rook, assetsRoot + "/Chess_Piece/rook.obj");
        loadPieceMesh(PieceMeshType::Decoration, assetsRoot + "/Chess_Piece/decoration.obj");
    }

public:
    explicit Renderer3D(const std::string& assetsRoot = "") {
        createCubeMesh();
        if (!assetsRoot.empty()) {
            loadPieceMeshes(assetsRoot);
        }
    }

    ~Renderer3D() {
        destroyMesh(m_cube);
        for (auto& mesh : m_pieceMeshes) {
            destroyMesh(mesh);
        }
    }

    void drawCube() const {
        drawMesh(m_cube);
    }

    void drawPiece(PieceMeshType type) const {
        const auto& mesh = m_pieceMeshes[static_cast<size_t>(type)];
        if (mesh.isValid()) {
            drawMesh(mesh);
        }
        else {
            drawCube();
        }
    }
};