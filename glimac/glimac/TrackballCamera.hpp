#pragma once

#include "glm.hpp"

namespace glimac {

class TrackballCamera {
    private:
        float m_fDistance;
        float m_fAngleX;
        float m_fAngleY;

    public:
        TrackballCamera()
        :m_fDistance(5), m_fAngleX(0), m_fAngleY(0)
        {};

        void moveFront(float delta) {
            m_fDistance += delta;
        }; 

        void rotateLeft(float degrees) {
            m_fAngleY += degrees;
        };

        void rotateUp(float degrees) {
            m_fAngleX += degrees;
        };

        glm::mat4 getViewMatrix() const {
            glm::mat4 view = glm::mat4(1.0f);

            // 1. Translation (on recule la scène → donc -distance)
            view = glm::translate(view, glm::vec3(0.0f, 0.0f, -m_fDistance));

            // 2. Rotation autour de X (angle vertical)
            view = glm::rotate(view, glm::radians(m_fAngleX), glm::vec3(1.0f, 0.0f, 0.0f));

            // 3. Rotation autour de Y (angle horizontal)
            view = glm::rotate(view, glm::radians(m_fAngleY), glm::vec3(0.0f, 1.0f, 0.0f));

            return view;
        };
};

}
