#include <iostream>
#include <vector>
#include <stb/stb_image_write.h>
#include "Random.h"
#include "Vector.h"
#include "Ray.h"
#include "Scene.h"
#include "Intersection.h"
#include "Camera.h"


namespace LtRenderer{

    static const std::string nee_mat_name = "Lambertion";

    Vec3 render(const Ray& ray, Scene& scene, Random* random, const int keep_depth)
    {
        Vec3 attenuation = Vec3(1.0);
        Vec3 result = Vec3(0.0);
        Ray scatterd_ray = Ray(ray.origin(), ray.direction());
        double russian_roulette_probability = 1.0;
        int depth = 0;
        while (true)
        {
			// attenuation‚ÌŠe—v‘f‚ª0.0‚Ì‚Æ‚«A‚»‚êˆÈã‚Ìloop‚ÍŠñ—^‚ªæ‚ê‚È‚¢‚½‚ßbreak‚·‚é
			if (attenuation.x() <= 0.0 && attenuation.y() <= 0.0 && attenuation.z() <= 0.0)
			{
				break;
			}

            Intersection intersection;
            if (!scene.intersectScene(scatterd_ray, &intersection))
            {
                //result += attenuation * scene.samplingIBL(scatterd_ray.direction());
                break;
            }

            if (intersection.material()->materialType() == nee_mat_name)
            {
                result += attenuation * scene.nextEventEstimation(&intersection, random);
            }

            result += intersection.material()->emission() * attenuation;
            double russian_roulette_probability = max(intersection.material()->albedo().x(),
                                                      max(intersection.material()->albedo().y(),
                                                          intersection.material()->albedo().z()));
            if (depth > keep_depth)
            {
                if (random->zeroToOneFloat() >= russian_roulette_probability)
                {
                    break;
                }
            }
            else
            {
                russian_roulette_probability = 1.0;
            }
            scatterd_ray = intersection.material()->scatter(scatterd_ray,
                                                            intersection.position(),
                                                            intersection.normal(),
                                                            intersection.uv(),
                                                            attenuation,
                                                            random);
            attenuation /= russian_roulette_probability;
            ++depth;
        }
        return result;
    }

}//namespace LtRenderer


int main(int argc, char** argv)
{
    int width   = 640;
    int height  = 480;
    int samples = 256;

    LtRenderer::Scene scene;
    LtRenderer::Camera camera = LtRenderer::Camera(LtRenderer::Vec3(0.0, 13, 50),
                                                   LtRenderer::normalize(LtRenderer::Vec3(0.0, 0.0, -1.0)),
                                                   LtRenderer::Vec3(0.0, 1.0, 0.0),
                                                   double(width) / double(height),
                                                   1.0);
    std::vector<uint8_t> image;
    image.resize(width * height * 3);

    double invert_gamma = 1.0 / LtRenderer::GAMMA_VALUE;
#pragma omp parallel for schedule(dynamic, 1) num_threads(7)
    for (int y = 0; y < height; ++y)
    {
        std::cout << "progress..." << double(y) / double(height) * 100.0 << "%" << std::endl;
        LtRenderer::Random random(y+1);
        for (int x = 0; x < width; ++x)
        {
            LtRenderer::Vec3 col = LtRenderer::Vec3(0.0);
            for (int sample = 0; sample < samples; ++sample)
            {
                double u = (double(x) + random.zeroToOneFloat()) / double(width) - 0.5;
                double v = (double(y) + random.zeroToOneFloat()) / double(height) - 0.5;
                col += render(camera.emit(u, v), scene, &random, 5);
            }
            col /= double(samples);
            int pixel_id = (y * width + x) * 3;
            image[pixel_id + 0] = static_cast<uint8_t>(LtRenderer::saturate(pow(col.x(), invert_gamma)) * 255.0 + 0.5);
            image[pixel_id + 1] = static_cast<uint8_t>(LtRenderer::saturate(pow(col.y(), invert_gamma)) * 255.0 + 0.5);
            image[pixel_id + 2] = static_cast<uint8_t>(LtRenderer::saturate(pow(col.z(), invert_gamma)) * 255.0 + 0.5);
        }
    }
    stbi_write_bmp("render_result.bmp", width, height, 3, image.data());
    image.clear();
}
