#include <iostream>
#include <vector>
#include "Random.h"
#include "Vector.h"
#include "Ray.h"
#include "Intersection.h"
#include "Scene.h"
#include "Intersection.h"
#include "Camera.h"

namespace LtRenderer{
	Vec3 render(const Ray& ray, Scene& scene, Random* random, const int keep_depth)
	{
		Vec3 attenuation = Vec3(1.0);
		Vec3 result = Vec3(0.0);
		Ray scatterd_ray = Ray(ray.origin(), ray.direction());
		double russian_roulette_probability = 1.0;
		const std::string nee_mat_name = "Lambertion";
		int depth = 0;
		while (true) 
		{
			Intersection intersection;
			if (!scene.bvhIntersectScene(scatterd_ray, &intersection))
			{
				// TODO IBL sampling
				break;
			}
			else
			{
				if (intersection.material()->materialType() == nee_mat_name)
				{
					result += attenuation * scene.nextEventEstimation(&intersection, random);
				}

				result += intersection.material()->emission() * attenuation;
				double russian_roulette_probability = max(intersection.material()->albedo().x(),
														 max(intersection.material()->albedo().y(),
															 intersection.material()->albedo().z()));
				// keep_depth‚ð’´‚¦‚È‚¢ŠÔ‚ÌÄ‹A‚Í•ÛØ
				// ’´‚¦‚½ê‡‚Írussian_roulette_probability‚Æ—”‚ð”äŠr‚µ‚Äˆ—‚ð‘±‚¯‚é‚©Œˆ‚ß‚é
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
									            				attenuation,
																random);
				attenuation /= russian_roulette_probability;
				++depth;
			}
		}
		return result;
	}

}//namespace LtRenderer


int main(int argc, char** argv)
{
	int width   = 640;
	int height  = 480;
	int samples = 128;

	std::cout << "P3\n" << width << " " << height << "\n255\n";

	LtRenderer::Scene scene;
	LtRenderer::Camera camera = LtRenderer::Camera(LtRenderer::Vec3(50.0, 50.0, 300),
												   LtRenderer::normalize(LtRenderer::Vec3(0.0, 0.0, -1.0)),
												   LtRenderer::Vec3(0.0, 1.0, 0.0),
												   double(width) / double(height),
												   130.0);
	std::vector<LtRenderer::Vec3> image;
	image.resize(width * height);

	double invert_gamma = 1.0 / LtRenderer::GAMMA_VALUE;
#pragma omp parallel for schedule(dynamic, 1) num_threads(7)
	for (int y = 0; y < height; ++y)
	{ 
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
			col = LtRenderer::Vec3(LtRenderer::saturate(pow(col.x(), invert_gamma)), 
								   LtRenderer::saturate(pow(col.y(), invert_gamma)), 
								   LtRenderer::saturate(pow(col.z(), invert_gamma)));
			
			int pixel_id = y * width + x;
			image[pixel_id] = LtRenderer::Vec3(int(255.99*col.x()),
										       int(255.99*col.y()),
										       int(255.99*col.z()));
		}
	}
	for (auto pixel : image)
	{
		std::cout << pixel.x() << " " << pixel.y() << " " << pixel.z() << " ";
	}
}
