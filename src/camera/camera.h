#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define MOVEMENT_KEYS 0
#define MOVE_FORWARD 0
#define MOVE_BACKWARD 1
#define MOVE_LEFT 2
#define MOVE_RIGHT 4

class Camera {
    private:
        float   YAW         = -90.0f,
                PITCH       =  0.0f,
                SPEED       =  2.5f,
                SENSITIVITY =  0.1f,
                FOV         =  50.0f;

        bool    MOUSE_INITIALIZED = false;

        double  LAST_MOUSE_X = 0.0f,
                LAST_MOUSE_Y = 0.0f;

        glm::vec3 POSITION{}, FRONT{}, UP{};

    public:
        Camera(glm::vec3 position, glm::vec3 front, glm::vec3 up);
        void handle_key(int action, float deltaTime);
        void handle_mouse(double x, double y);
        float get_fov() const;
        glm::mat4 get_view_matrix();
        glm::vec3 get_position();
        glm::vec3 get_front();
};


#endif
