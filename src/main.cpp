#include <fstream>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

using namespace std;

int main() {

    // image size
    const int height = 800;
    const int width = 1280;

    // init framebuffer
    glm::vec3 *frame = (glm::vec3*) malloc(height * width * sizeof(glm::vec3));

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            frame[i*width + j] = glm::vec3(float(i) / float(height), float(j) / float(width), 0.0f);
        }
    }

    // save to ppm
    ofstream file;
    file.open("render.ppm");
    file << "P3 " << width << " " << height << " 255\n";

    glm::ivec3 pixel;
    for (int i = 0; i < height * width; i++) {
        pixel = glm::floor(255.0f * glm::clamp(frame[i], 0.0f, 1.0f));
        file << pixel.x << " " << pixel.y << " " << pixel.z << "\n";
    }

    file.close();

}