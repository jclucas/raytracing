#include <fstream>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "camera.h"

using namespace std;

int main() {

    // image size
    const int height = 800;
    const int width = 1280;

    Camera camera = Camera(glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), glm::vec3(0, 0, 1));

    // init framebuffer
    glm::vec3 *frame = camera.render(height, width, Scene());

    // save to ppm
    ofstream file;
    file.open("render.ppm");
    file << "P3 " << width << " " << height << " 255\n";

    glm::ivec3 pixel;
    for (int i = 0; i < height * width; i++) {
        pixel = glm::floor(255.0f * glm::clamp(frame[i], 0.0f, 1.0f));
        file << pixel.x << " " << pixel.y << " " << pixel.z << "\n";
    }

    delete[] frame;
    file.close();

}