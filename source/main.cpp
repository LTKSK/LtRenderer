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
		float russian_roulette_probability = 1.0f;
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
				float russian_roulette_probability = max(intersection.material()->albedo().x(),
														 max(intersection.material()->albedo().y(),
															 intersection.material()->albedo().z()));
				// keep_depthを超えない間の再帰は保証
				// 超えた場合はrussian_roulette_probabilityと乱数を比較して処理を続けるか決める
				if (depth > keep_depth)
				{
					if (random->zeroToOneFloat() >= russian_roulette_probability)
					{
						break;
					}
				}
				else
				{
					russian_roulette_probability = 1.0f;
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
	int samples = 1024;

	std::cout << "P3\n" << width << " " << height << "\n255\n";

	LtRenderer::Scene scene;
	LtRenderer::Camera camera = LtRenderer::Camera(LtRenderer::Vec3(50.0, 50.0, 300),
												   LtRenderer::normalize(LtRenderer::Vec3(0.0, 0.0, -1.0)),
												   LtRenderer::Vec3(0.0, 1.0, 0.0),
												   float(width) / float(height),
												   130.0);
	
	float invert_gamma = 1.0 / LtRenderer::GAMMA_VALUE;
	for (int y = 0; y < height; ++y)
	{ 
		LtRenderer::Random random(y+1);
		for (int x = 0; x < width; ++x)
		{
			LtRenderer::Vec3 col = LtRenderer::Vec3(0.0);
#pragma omp parallel for schedule(dynamic, 1) num_threads(4)
			for (int sample = 0; sample < samples; ++sample)
			{
				float u = (float(x) + random.zeroToOneFloat()) / float(width) - 0.5;
				float v = (float(y) + random.zeroToOneFloat()) / float(height) - 0.5;
				//cameraの原点から、horizontalとverticalで定義された範囲の中へのvector
				LtRenderer::Ray ray = camera.emit(u, v);
	
				col += render(ray, scene, &random, 5);
			}
			col /= float(samples);
			col = LtRenderer::Vec3(LtRenderer::saturate(pow(col.x(), invert_gamma)), 
								   LtRenderer::saturate(pow(col.y(), invert_gamma)), 
								   LtRenderer::saturate(pow(col.z(), invert_gamma)));
			int image_r = int(255.99*col.x());
			int image_g = int(255.99*col.y());
			int image_b = int(255.99*col.z());
			std::cout << image_r << " " << image_g << " " << image_b << " ";
		}
	}
}
