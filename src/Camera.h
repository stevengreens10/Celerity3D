#ifndef GUI_CAMERA_H
#define GUI_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {

private:
    inline static glm::vec3 cameraPos;
    inline static glm::vec3 cameraDir;
    inline static float yaw;
    inline static float pitch;
    inline static glm::mat4 viewMatrix;
public:
    static void Translate(glm::vec3 translation);

    static void Rotate(float _yaw, float _pitch);

    // Updates camera based on mouse displacement in screen coords
    static void Displace(int x, int y);

    inline static const glm::vec3 &Pos() { return cameraPos; }

    inline static const glm::vec3 &Dir() { return cameraDir; }

    inline static float Yaw() { return yaw; }

    inline static float Pitch() { return pitch; }

    inline static const glm::mat4 &ViewMatrix() { return viewMatrix; }

private:
    static void updateViewMatrix();

};


#endif //GUI_CAMERA_H
