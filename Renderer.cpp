#include <fstream>
#include "Scene.hpp"
#include "Renderer.hpp"


inline float deg2rad(const float& deg) { return deg * M_PI / 180.0; }

const float EPSILON = 0.00001;
//const float EPSILON = 0.0001;

Renderer::Renderer(std::vector<std::vector<double>>& v) : sobol_sequence(v), sobol_sequence_counter(0){
    // for (auto x : v) {
    //     std::cout << x[0] << "      " << x[1] << std::endl;
    // }
}

std::vector<double> Renderer::getSobolRandom() {
    // each pair of sobol sequence are two numbers between 0 and 1
    // we want the output to be a pair between -0.5 and 0.5
    if (sobol_sequence_counter == sobol_sequence.size() - 1)
        sobol_sequence_counter = 0;
    std::vector<double> result = sobol_sequence[sobol_sequence_counter++];
    result[0] -= 0.5;
    result[1] -= 0.5;
    return result;
    // return sobol_sequence[sobol_sequence_counter++];
}

// The main render function. This where we iterate over all pixels in the image,
// generate primary rays and cast these rays into the scene. The content of the
// framebuffer is saved to a file.
void Renderer::Render(const Scene& scene)
{
    std::vector<Vector3f> framebuffer(scene.width * scene.height);

    float scale = tan(deg2rad(scene.fov * 0.5));
    float imageAspectRatio = scene.width / (float)scene.height;
    Vector3f eye_pos(278, 273, -800);
    int m = 0;

    // for submission, render 4 images with 1, 4, 16, and 64 spp
    // change the spp value to change number of path samples per pixel
    int spp = 128;
    int spsp = spp / 4;
    // std::cout << "SPP: " << spp << "\n";
    for (uint32_t j = 0; j < scene.height; ++j) {
        for (uint32_t i = 0; i < scene.width; ++i) {
            // generate primary ray direction
            // float j_1 = j - 0.5; float j_2 = j + 0.5;
            // float i_1 = i - 0.5; float i_2 = j + 0.5;
            // if (j_1 < 0) j_1 = 0;
            // if (j_2 > scene.height - 1) j_2 = scene.height - 1;
            // if (i_1 < 0) i_1 = 0;
            // if (i_2 > scene.width - 1) i_2 = scene.width - 1;
            // for (int k = 0; k < spsp; ++k) {
            //     auto temp = getSobolRandom();
            //     float sy = j_1 + temp[0]; if (sy < 0) sy = 0;
            //     float sx = i_1 + temp[1]; if (sx < 0) sx = 0;
            //     float x = (2 * (sx + 0.5) / (float)scene.width - 1) *
            //           imageAspectRatio * scale;
            //     float y = (1 - 2 * (sy + 0.5) / (float)scene.height) * scale;
            //     Vector3f dir = normalize(Vector3f(-x, y, 1));
            //     framebuffer[m] += scene.castRay(Ray(eye_pos, dir)) / spp; 
            // }
            // for (int k = 0; k < spsp; ++k) {
            //     auto temp = getSobolRandom();
            //     float sy = j_1 + temp[0]; if (sy < 0) sy = 0;
            //     float sx = i_2 + temp[1]; if (sx > scene.width - 1) sx = scene.width - 1;
            //     float x = (2 * (sx + 0.5) / (float)scene.width - 1) *
            //           imageAspectRatio * scale;
            //     float y = (1 - 2 * (sy + 0.5) / (float)scene.height) * scale;
            //     Vector3f dir = normalize(Vector3f(-x, y, 1));
            //     framebuffer[m] += scene.castRay(Ray(eye_pos, dir)) / spp; 
            // }
            // for (int k = 0; k < spsp; ++k) {
            //     auto temp = getSobolRandom();
            //     float sy = j_2 + temp[0]; if (sy > scene.height - 1) sy = scene.height - 1;
            //     float sx = i_1 + temp[1]; if (sx < 0) sx = 0;
            //     float x = (2 * (sx + 0.5) / (float)scene.width - 1) *
            //           imageAspectRatio * scale;
            //     float y = (1 - 2 * (sy + 0.5) / (float)scene.height) * scale;
            //     Vector3f dir = normalize(Vector3f(-x, y, 1));
            //     framebuffer[m] += scene.castRay(Ray(eye_pos, dir)) / spp; 
            // }
            // for (int k = 0; k < spsp; ++k) {
            //     auto temp = getSobolRandom();
            //     float sy = j_2 + temp[0]; if (sy > scene.height - 1) sy = scene.height - 1;
            //     float sx = i_2 + temp[1]; if (sx > scene.width - 1) sx = scene.width - 1;
            //     float x = (2 * (sx + 0.5) / (float)scene.width - 1) *
            //           imageAspectRatio * scale;
            //     float y = (1 - 2 * (sy + 0.5) / (float)scene.height) * scale;
            //     Vector3f dir = normalize(Vector3f(-x, y, 1));
            //     framebuffer[m] += scene.castRay(Ray(eye_pos, dir)) / spp; 
            // }
            // float x = (2 * (i + 0.5) / (float)scene.width - 1) *
            //           imageAspectRatio * scale;
            // float y = (1 - 2 * (j + 0.5) / (float)scene.height) * scale;

            // Vector3f dir = normalize(Vector3f(-x, y, 1));
            for (int k = 0; k < spp; k++){
                auto temp = getSobolRandom();
                float sy = j + temp[0]; 
                float sx = i + temp[1];
                float x = (2 * (sx + 0.5) / (float)scene.width - 1) *
                      imageAspectRatio * scale;
                float y = (1 - 2 * (sy + 0.5) / (float)scene.height) * scale;
                Vector3f dir = normalize(Vector3f(-x, y, 1));
                framebuffer[m] += scene.castRay(Ray(eye_pos, dir), 0) / spp;  
            }
            m++;
        }
        UpdateProgress(j / (float)scene.height);
    }
    UpdateProgress(1.f);

    // save framebuffer to file
    FILE* fp = fopen("binary.ppm", "wb");
    (void)fprintf(fp, "P6\n%d %d\n255\n", scene.width, scene.height);
    for (auto i = 0; i < scene.height * scene.width; ++i) {
        static unsigned char color[3];
        
        // TODO: Implement correct gamma correction 
        //       (by modifying the next three lines)
        float coefficient = 1.0 / 2.2;
        color[0] = (unsigned char) (255 * std::pow(clamp(0, 1, framebuffer[i].x), coefficient));
        color[1] = (unsigned char) (255 * std::pow(clamp(0, 1, framebuffer[i].y), coefficient));
        color[2] = (unsigned char) (255 * std::pow(clamp(0, 1, framebuffer[i].z), coefficient));

        fwrite(color, 1, 3, fp);
    }
    fclose(fp);    
}
