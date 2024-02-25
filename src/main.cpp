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
#include "Regular.h"

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

void build_test_scene(hittable_list& world) {
	plane* plane_model = new plane();
	material* plane_material_model = new material(color(0.5f, 0.5f, 0.5f), color(0.5f, 0.5f, 0.5f), color(0.5f, 0.5f, 0.5f), 1.0f);
	auto plane_instance_ptr = make_shared<instance>(plane_model, plane_material_model);
	world.add(plane_instance_ptr);

	sphere* sphere_model = new sphere();
	material* sphere_material_model = new material(color(1.0f, 0.65f, 0.0f), color(1.0f, 0.65f, 0.0f), color(1.0f, 0.65f, 0.0f), 1.0f);
	auto sphere_instance_ptr = make_shared<instance>(sphere_model, sphere_material_model);
	sphere_instance_ptr->translate(0.0f, 1.0f, 0.0f);
	world.add(sphere_instance_ptr);
}

void build_Cornell_Box(hittable_list& world) {
	// Material
	material* white_material_model = new material(color(1.0f, 1.0f, 1.0f), color(1.0f, 1.0f, 1.0f), color(1.0f, 1.0f, 1.0f), 0.5f);
	material* red_material_model = new material(color(1.0f, 0.0f, 0.0f), color(1.0f, 0.0f, 0.0f), color(1.0f, 0.0f, 0.0f), 0.5f);
	material* green_material_model = new material(color(0.0f, 1.0f, 0.0f), color(0.0f, 1.0f, 0.0f), color(0.0f, 1.0f, 0.0f), 0.5f);
	material* dirty_white_material_model = new material(color(0.7f, 0.7f, 0.7f), color(0.7f, 0.7f, 0.7f), color(0.7f, 0.7f, 0.7f), 0.5f);
	material* warm_white_material_model = new material(color(1.0f, 0.95f, 0.9f), color(1.0f, 0.95f, 0.9f), color(1.0f, 0.95f, 0.9f), 0.5f);
	material* light_beige_material_model = new material(color(0.95f, 0.9f, 0.8f), color(0.95f, 0.9f, 0.8f), color(0.95f, 0.9f, 0.8f), 0.5f);

	// Left Wall
	quadrilateral* left_wall_model = new quadrilateral(point3(0.0f, 0.0f, 10.0f), point3(0.0f, 0.0f, 0.0f), point3(0.0f, 10.0f, 0.0f), point3(0.0f, 10.0f, 10.0f));
	auto left_wall_instance_ptr = make_shared<instance>(left_wall_model, red_material_model);
	world.add(left_wall_instance_ptr);
	// Right Wall
	quadrilateral* right_wall_model = new quadrilateral(point3(10.0f, 0.0f, 10.0f), point3(10.0f, 0.0f, 0.0f), point3(10.0f, 10.0f, 0.0f), point3(10.0f, 10.0f, 10.0f));
	auto right_wall_instance_ptr = make_shared<instance>(right_wall_model, green_material_model);
	world.add(right_wall_instance_ptr);
	// Back Wall
	quadrilateral* back_wall_model = new quadrilateral(point3(0.0f, 0.0f, 0.0f), point3(0.0f, 10.0f, 0.0f), point3(10.0f, 10.0f, 0.0f), point3(10.0f, 0.0f, 0.0f));
	auto back_wall_instance_ptr = make_shared<instance>(back_wall_model, light_beige_material_model);
	world.add(back_wall_instance_ptr);
	// Ceiling
	quadrilateral* ceiling_model = new quadrilateral(point3(0.0f, 10.0f, 10.0f), point3(10.0f, 10.0f, 10.0f), point3(10.0f, 10.0f, 0.0f), point3(0.0f, 10.0f, 0.0f));
	auto ceiling_instance_ptr = make_shared<instance>(ceiling_model, light_beige_material_model);
	world.add(ceiling_instance_ptr);
	// Flooring
	quadrilateral* flooring_model = new quadrilateral(point3(0.0f, 0.0f, 10.0f), point3(10.0f, 0.0f, 10.0f), point3(10.0f, 0.0f, 0.0f), point3(0.0f, 0.0f, 0.0f));
	auto flooring_instance_ptr = make_shared<instance>(flooring_model, light_beige_material_model);
	world.add(flooring_instance_ptr);
	// First Box
	box* first_box_model = new box(point3(2.0f, 0.0f, 2.0f), point3(5.0f, 6.5f, 5.0f));
	auto first_box_instance_ptr = make_shared<instance>(first_box_model, light_beige_material_model);
	first_box_instance_ptr->rotate_y(12.5);
	first_box_instance_ptr->translate(vec3(-1.0f, 0.0f, 0.0f));
	first_box_instance_ptr->translate(vec3(0.0f, 0.0f, 2.0f));
	world.add(first_box_instance_ptr);
	// Second Box
	box* second_box_model = new box(point3(4.0f, 0.0f, 4.0f), point3(7.5f, 3.5f, 7.5f));
	auto second_box_instance_ptr = make_shared<instance>(second_box_model, light_beige_material_model);
	second_box_instance_ptr->rotate_y(-13);
	second_box_instance_ptr->translate(vec3(2.25f, 0.0f, 0.0f));
	second_box_instance_ptr->translate(vec3(0.0f, 0.0f, 0.5f));
	world.add(second_box_instance_ptr);
	// Area Light
	quadrilateral* area_light_model = new quadrilateral(point3(4.0f, 9.999f, 6.0f), point3(6.0f, 9.999f, 6.0f), point3(6.0f, 9.999f, 4.0f), point3(4.0f, 9.999f, 4.0f));
	auto area_light_instance_ptr = make_shared<instance>(area_light_model, warm_white_material_model);
	world.add(area_light_instance_ptr);
}

int main(int argc, char* argv[]){
	// World
	hittable_list world;

	int num_samples = 1024;
	//Regular* sample_ptr = new Regular(num_samples);
	MultiJittered* sample_ptr = new MultiJittered(num_samples);
	AmbientOccluder* ambient_occluder_ptr = new AmbientOccluder();
	ambient_occluder_ptr->set_sampler(sample_ptr);

	//build_test_scene(world);
	build_Cornell_Box(world);

	color lightgray = color(0.75f, 0.75f, 0.75f);
	point3 light_position(0.0f, 10.0f, 0.0f);
	point_light* worldlight = new point_light(light_position, lightgray, lightgray, lightgray);

	camera cam;
	cam.lookfrom = point3(5.0f, 6.0f, 40.0f);
	cam.lookat = point3(5.0f, 5.0f, 5.0f);

	cam.aspect_ratio = 16.0f / 9.0f;
	cam.image_width = 1920;
	cam.samples_per_pixel = 256;
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
