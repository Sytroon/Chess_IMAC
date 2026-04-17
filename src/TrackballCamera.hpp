#pragma once
#include "glimac/glm.hpp"

enum CameraMode {
    TRACKBALL,
    PIECE_VIEW
};

class TrackballCamera {
private:
    // X = left/right, Y = height, Z = depth.
    float m_fDistance; // Distance to center
    float m_fAngleX;   // Elevation
    float m_fAngleY;   // Rotation
    glm::vec3 m_Target; // target
    CameraMode m_Mode;
    glm::vec3 m_PiecePosition;

public:
    TrackballCamera() 
        : m_fDistance(12.0f), m_fAngleX(45.0f), m_fAngleY(45.0f), m_Target(3.5f, 0.0f, 3.5f), m_Mode(TRACKBALL), m_PiecePosition(0.0f, 0.0f, 0.0f) {}

    // Move camera with mouse
    void moveFront(float delta) { 
        if (m_Mode == TRACKBALL) {
            m_fDistance = glm::clamp(m_fDistance - delta, 2.0f, 30.0f); 
        }
    }
    void rotateLeft(float degrees) { m_fAngleY += degrees; }
    void rotateUp(float degrees) { 
        if (m_Mode == TRACKBALL) {
            m_fAngleX = glm::clamp(m_fAngleX + degrees, 10.0f, 85.0f); 
        } else {
            m_fAngleX += degrees;
        }
    }

    void setMode(CameraMode mode) { m_Mode = mode; }
    void setPiecePosition(const glm::vec3& pos) { m_PiecePosition = pos; }
    CameraMode getMode() const { return m_Mode; }

    glm::mat4 getViewMatrix() const {
        if (m_Mode == TRACKBALL) {
            glm::mat4 ViewMatrix = glm::mat4(1.0f);
            // 1. Put camera back
            ViewMatrix = glm::translate(ViewMatrix, glm::vec3(0.0f, 0.0f, -m_fDistance));
            // 2. Rotations
            ViewMatrix = glm::rotate(ViewMatrix, glm::radians(m_fAngleX), glm::vec3(1, 0, 0));
            ViewMatrix = glm::rotate(ViewMatrix, glm::radians(m_fAngleY), glm::vec3(0, 1, 0));
            // 3. Move to target
            ViewMatrix = glm::translate(ViewMatrix, -m_Target);
            return ViewMatrix;
        } else { // PIECE_VIEW
            glm::mat4 ViewMatrix = glm::mat4(1.0f);
            // Rotations
            ViewMatrix = glm::rotate(ViewMatrix, glm::radians(m_fAngleX), glm::vec3(1, 0, 0));
            ViewMatrix = glm::rotate(ViewMatrix, glm::radians(m_fAngleY), glm::vec3(0, 1, 0));
            ViewMatrix = glm::translate(ViewMatrix, glm::vec3(0, -2, 0));
            // Move to target
            ViewMatrix = glm::translate(ViewMatrix, -m_PiecePosition);
            return ViewMatrix;
        }
    }
};