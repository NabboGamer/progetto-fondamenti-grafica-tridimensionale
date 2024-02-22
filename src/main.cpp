#include <iostream>
#include <time.h>
#include <stdio.h>
#include "SDL.h" 
#include "SDL_image.h"
#include "float.h"
#include "vec3.h"
#include "color.h"
#include "ray.h"
#include "geometry.h"
#include "cylinder.h"
#include "sphere.h"
#include "hittable.h"
#include "hittable_list.h"
#include "instance.h"
#include "mesh.h"
#include "raster.h"
#include "object.h"
#include "camera.h"
#include "color.h"
#include "texture.h"
#include "Multijittered.h"

const unsigned int MAX_RAY_DEPTH = 0;

using namespace std;

int init(int width, int height) {
	/* // Initialize SDL2. */
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	/* Create the window where we will draw. */
	window = SDL_CreateWindow("Ambient Occlusion", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
	if (window == nullptr) {
		cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	/* We must call SDL_CreateRenderer in order for draw calls to affect this window. */
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == nullptr) {
		SDL_DestroyWindow(window);
		cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}
	return 0;
}


void close() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	IMG_Quit();
	SDL_Quit();
}

int main(int argc, char* argv[]){
	// World
	hittable_list world;

	int num_samples = 1;
	MultiJittered* sample_ptr = new MultiJittered(num_samples);
	AmbientOccluder* ambient_occluder_ptr = new AmbientOccluder();
	ambient_occluder_ptr->set_sampler(sample_ptr);

	/*cout << "Numero di samples: " << ambient_occluder_ptr->sampler_ptr->get_num_samples() << "\n";
	auto hemisphere_samples = ambient_occluder_ptr->sampler_ptr->hemisphere_samples;
	for (int i = 0; i < hemisphere_samples.size(); i++){
		cout << "(" << hemisphere_samples[i].e[0] << "," << hemisphere_samples[i].e[1] << "," << hemisphere_samples[i].e[2] << ")\n";
	}*/

	plane* plane_model = new plane();
	material* plane_material_model = new material(color(0.5f, 0.5f, 0.5f), color(0.5f, 0.5f, 0.5f), color(0.5f, 0.5f, 0.5f), 1.0f);
	auto plane_instance_ptr = make_shared<instance>(plane_model, plane_material_model);
	world.add(plane_instance_ptr);

	sphere* sphere_model = new sphere();
	material* sphere_material_model = new material(color(0.8f, 0.6f, 0.1f), color(0.8f, 0.6f, 0.1f), color(0.8f, 0.6f, 0.1f), 1.0f);
	auto sphere_instance_ptr = make_shared<instance>(sphere_model, sphere_material_model);
	sphere_instance_ptr->translate(0.0f, 1.0f, 0.0f);
	world.add(sphere_instance_ptr);

	color lightgray = color(0.75f, 0.75f, 0.75f);
	point3 light_position(0.0f, 10.0f, 0.0f);
	point_light* worldlight = new point_light(light_position, lightgray, lightgray, lightgray);

	camera cam;
	cam.lookfrom = point3(10.0f, 5.0f, 10.0f);
	cam.lookat = point3(0.0f, 0.5f, 0.0f);

	cam.aspect_ratio = 16.0f / 9.0f;
	cam.image_width = 960;
	cam.samples_per_pixel = 1;
	cam.vfov = 20;

	cam.initialize();

	if (init(cam.image_width, cam.image_height) == 1) {
		cout << "App Error! " << std::endl;
		return 1;
	}

	cout << "Image Resolution: " << cam.image_width << "x" << cam.image_height << "\n";

	time_t start, end;
	time(&start);

	cam.parallel_render(world, *worldlight, ambient_occluder_ptr);
	//cam.render(world, *worldlight);

	time(&end);
	double dif = difftime(end, start);
	cout << "\n" << "Rendering time: " << dif << "\n";

	SDL_Event event;
	bool quit = false;

	/* Poll for events */
	while (SDL_PollEvent(&event) || (!quit)) {

		switch (event.type) {

		case SDL_QUIT:
			quit = true;
			break;

		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE:
				quit = true;
				break;
				// cases for other keypresses

			case SDLK_s:
				saveScreenshotBMP("../render/screenshot.bmp");
				cout << "Screenshot saved!" << endl;
				break;
			}
		}
	}

	close();
	return 0;
}
