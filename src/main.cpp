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

const unsigned int MAX_RAY_DEPTH = 5;

using namespace std;

int init(int width, int height) {
	/* // Initialize SDL2. */
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	/* Create the window where we will draw. */
	window = SDL_CreateWindow("Phong Shading", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
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

int main(int argc, char* argv[])
{
	// World
	hittable_list world;

	color lightgray = color(0.75f, 0.75f, 0.75f);
	point3 light_position(6.0f, 6.0f, 0.0f);
	point_light* worldlight = new point_light(light_position, lightgray, lightgray, lightgray);

	srand(10);
	
	sphere* sphere_model = new sphere(); //posizione (0,0,0) e raggio 1.0
	auto instance_ptr = make_shared<instance>(sphere_model, new material());
	instance_ptr->scale(1000.0f, 1000.0f, 1000.0f);
	instance_ptr->translate(0.0f, -1000.0f, 0.0f);
	instance_ptr->getMaterial()->reflective = 0.5f;
	world.add(instance_ptr);

	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			point3 center(a + 0.9f * random_float(), 0.2f, b + 0.9f * random_float());
			instance_ptr = make_shared<instance>(sphere_model, new material());
			instance_ptr->scale(0.2f, 0.2f, 0.2f);
			instance_ptr->translate(center[0], center[1], center[2]);
			instance_ptr->getMaterial()->reflective = 0.5f;
			world.add(instance_ptr);
		}
	}

	instance_ptr = make_shared<instance>(sphere_model, new material());
	instance_ptr->translate(0.0f, 1.0f, 0.0f);
	instance_ptr->getMaterial()->reflective = 0.2f;
	world.add(instance_ptr);

	instance_ptr = make_shared<instance>(sphere_model, new material());
	instance_ptr->translate(-4.0f, 1.0f, 0.0f);
	instance_ptr->getMaterial()->reflective = 0.5f;
	world.add(instance_ptr);

	instance_ptr = make_shared<instance>(sphere_model, new material());
	instance_ptr->translate(4.0f, 1.0f, 0.0f);
	instance_ptr->getMaterial()->reflective = 1.0f;
	world.add(instance_ptr);

	camera cam;
	cam.lookfrom = point3(13, 5, 3);
	cam.lookat = point3(0, 0, 0);

	cam.aspect_ratio = 16.0f / 9.0f;
	cam.image_width = 960;
	cam.samples_per_pixel = 4;
	cam.vfov = 20;

	cam.initialize();

	if (init(cam.image_width, cam.image_height) == 1) {
		cout << "App Error! " << std::endl;
		return 1;
	}

	cout << "Image Resolution: " << cam.image_width << "x" << cam.image_height << "\n255\n";

	time_t start, end;
	time(&start);

	cam.parallel_render(world, *worldlight);
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
				saveScreenshotBMP("screenshot.bmp");
				cout << "Screenshot saved!" << endl;
				break;
			}
		}
	}

	close();
	return 0;
}
