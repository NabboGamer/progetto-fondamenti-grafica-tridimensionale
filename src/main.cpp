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
	plane_material_model->texture = new constant_texture(color(0.5f, 0.5f, 0.5f));
	auto plane_instance_ptr = make_shared<instance>(plane_model, plane_material_model);
	world.add(plane_instance_ptr);

	sphere* sphere_model = new sphere();
	material* sphere_material_model = new material(color(1.0f, 0.65f, 0.0f), color(1.0f, 0.65f, 0.0f), color(1.0f, 0.65f, 0.0f), 1.0f);
	sphere_material_model->texture = new constant_texture(color(1.0f, 0.65f, 0.0f));
	auto sphere_instance_ptr = make_shared<instance>(sphere_model, sphere_material_model);
	sphere_instance_ptr->translate(0.0f, 1.0f, 0.0f);
	world.add(sphere_instance_ptr);
}

void build_Cornell_Box(hittable_list& world) {
	// Material
	material* white_material_model = new material(color(1.0f, 1.0f, 1.0f), color(1.0f, 1.0f, 1.0f), color(1.0f, 1.0f, 1.0f), 0.5f);
	white_material_model->texture = new constant_texture(color(1.0f, 1.0f, 1.0f));

	material* red_material_model = new material(color(1.0f, 0.0f, 0.0f), color(1.0f, 0.0f, 0.0f), color(1.0f, 0.0f, 0.0f), 0.5f);
	red_material_model->texture = new constant_texture(color(1.0f, 0.0f, 0.0f));

	material* green_material_model = new material(color(0.0f, 1.0f, 0.0f), color(0.0f, 1.0f, 0.0f), color(0.0f, 1.0f, 0.0f), 0.5f);
	green_material_model->texture = new constant_texture(color(0.0f, 1.0f, 0.0f));

	material* dirty_white_material_model = new material(color(0.7f, 0.7f, 0.7f), color(0.7f, 0.7f, 0.7f), color(0.7f, 0.7f, 0.7f), 0.5f);
	dirty_white_material_model->texture = new constant_texture(color(0.7f, 0.7f, 0.7f));

	material* warm_white_material_model = new material(color(1.0f, 0.95f, 0.9f), color(1.0f, 0.95f, 0.9f), color(1.0f, 0.95f, 0.9f), 0.5f);
	warm_white_material_model->texture = new constant_texture(color(1.0f, 0.95f, 0.9f));

	material* light_beige_material_model = new material(color(0.95f, 0.9f, 0.8f), color(0.95f, 0.9f, 0.8f), color(0.95f, 0.9f, 0.8f), 0.5f);
	light_beige_material_model->texture = new constant_texture(color(0.95f, 0.9f, 0.8f));

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

void build_Compleat_Angler(hittable_list& world) {
	// Flooring
	quadrilateral* flooring_model = new quadrilateral(point3(0.0f, 0.0f, 10.0f), point3(2.0f, 0.0f, 10.0f), point3(2.0f, 0.0f, 0.0f), point3(0.0f, 0.0f, 0.0f));
	material* flooring_material = new material();
	flooring_material->texture = new checker_texture(new constant_texture(color(1.0f, 0.0f, 0.0f)), new constant_texture(color(1.0f, 1.0f, 0.0f)));
	auto flooring_instance_ptr = make_shared<instance>(flooring_model, flooring_material);
	world.add(flooring_instance_ptr);

	material* sphere_material = new material(color(0.9f, 0.9f, 0.9f), color(0.9f, 0.9f, 0.9f), color(0.9f, 0.9f, 0.9f), 0.5f);
	sphere_material->texture = new constant_texture(color(0.9f, 0.9f, 0.9f));
	// First Sphere
	sphere* first_sphere_model = new sphere(point3(0.5f, 0.4f, 8.5f), 0.25f);
	auto first_sphere_instance_ptr = make_shared<instance>(first_sphere_model, sphere_material);
	world.add(first_sphere_instance_ptr);

	// Second Sphere
	sphere* second_sphere_model = new sphere(point3(0.5f, 0.3f, 8.5f), 0.20f);
	auto second_sphere_instance_ptr = make_shared<instance>(second_sphere_model, sphere_material);
	second_sphere_instance_ptr->translate(0.3f, -0.05f, -0.3f);
	world.add(second_sphere_instance_ptr);
}

void build_The_Juggler(hittable_list& world) {
	// Flooring
	quadrilateral* flooring_model = new quadrilateral(point3(-1000.0f, 0.0f, 1000.0f), point3(1000.0f, 0.0f, 1000.0f), point3(1000.0f, 0.0f, -1000.0f), point3(-1000.0f, 0.0f, -1000.0f));
	material* flooring_material = new material();
	flooring_material->texture = new checker_texture(new constant_texture(color(1.0f, 1.0f, 0.0f)), new constant_texture(color(0.0f, 1.0f, 0.0f)));
	auto flooring_instance_ptr = make_shared<instance>(flooring_model, flooring_material);
	world.add(flooring_instance_ptr);

	material* sphere_material = new material(color(1.0f, 0.8f, 0.6f), color(1.0f, 0.8f, 0.6f), color(1.0f, 0.8f, 0.6f), 0.5f);
	sphere_material->texture = new constant_texture(color(1.0f, 0.8f, 0.6f));
	// Arts
	sphere* right_leg_sphere_1_model = new sphere(point3(22.0f, 0.0f, 14.5f), 0.1f);
	auto right_leg_sphere_1_instance_ptr = make_shared<instance>(right_leg_sphere_1_model, sphere_material);
	world.add(right_leg_sphere_1_instance_ptr);
	sphere* right_leg_sphere_2_model = new sphere(point3(22.0f, 0.3f, 14.5f), 0.2f);
	auto right_leg_sphere_2_instance_ptr = make_shared<instance>(right_leg_sphere_2_model, sphere_material);
	world.add(right_leg_sphere_2_instance_ptr);
	sphere* right_leg_sphere_3_model = new sphere(point3(21.9f, 0.8f, 14.4f), 0.3f);
	auto right_leg_sphere_3_instance_ptr = make_shared<instance>(right_leg_sphere_3_model, sphere_material);
	world.add(right_leg_sphere_3_instance_ptr);
	sphere* right_leg_sphere_4_model = new sphere(point3(21.8f, 1.4f, 14.3f), 0.3f);
	auto right_leg_sphere_4_instance_ptr = make_shared<instance>(right_leg_sphere_4_model, sphere_material);
	world.add(right_leg_sphere_4_instance_ptr);
	sphere* right_leg_sphere_5_model = new sphere(point3(21.7f, 2.0f, 14.2f), 0.3f);
	auto right_leg_sphere_5_instance_ptr = make_shared<instance>(right_leg_sphere_5_model, sphere_material);
	world.add(right_leg_sphere_5_instance_ptr);
	sphere* right_leg_sphere_6_model = new sphere(point3(21.6f, 2.6f, 14.1f), 0.3f);
	auto right_leg_sphere_6_instance_ptr = make_shared<instance>(right_leg_sphere_6_model, sphere_material);
	world.add(right_leg_sphere_6_instance_ptr);
	sphere* right_leg_sphere_7_model = new sphere(point3(21.5f, 3.2f, 14.0f), 0.3f);
	auto right_leg_sphere_7_instance_ptr = make_shared<instance>(right_leg_sphere_7_model, sphere_material);
	world.add(right_leg_sphere_7_instance_ptr);
	sphere* right_leg_sphere_8_model = new sphere(point3(21.4f, 3.8f, 13.9f), 0.3f);
	auto right_leg_sphere_8_instance_ptr = make_shared<instance>(right_leg_sphere_8_model, sphere_material);
	world.add(right_leg_sphere_8_instance_ptr);
	sphere* right_leg_sphere_9_model = new sphere(point3(21.3f, 4.5f, 13.8f), 0.4f);
	auto right_leg_sphere_9_instance_ptr = make_shared<instance>(right_leg_sphere_9_model, sphere_material);
	world.add(right_leg_sphere_9_instance_ptr);
	sphere* right_leg_sphere_10_model = new sphere(point3(21.4f, 5.1f, 13.9f), 0.3f);
	auto right_leg_sphere_10_instance_ptr = make_shared<instance>(right_leg_sphere_10_model, sphere_material);
	world.add(right_leg_sphere_10_instance_ptr);
	sphere* right_leg_sphere_11_model = new sphere(point3(21.5f, 5.6f, 14.0f), 0.3f);
	auto right_leg_sphere_11_instance_ptr = make_shared<instance>(right_leg_sphere_11_model, sphere_material);
	world.add(right_leg_sphere_11_instance_ptr);
	sphere* right_leg_sphere_12_model = new sphere(point3(21.6f, 6.1f, 14.1f), 0.3f);
	auto right_leg_sphere_12_instance_ptr = make_shared<instance>(right_leg_sphere_12_model, sphere_material);
	world.add(right_leg_sphere_12_instance_ptr);
	sphere* right_leg_sphere_13_model = new sphere(point3(21.7f, 6.6f, 14.2f), 0.3f);
	auto right_leg_sphere_13_instance_ptr = make_shared<instance>(right_leg_sphere_13_model, sphere_material);
	world.add(right_leg_sphere_13_instance_ptr);
	sphere* right_leg_sphere_14_model = new sphere(point3(21.8f, 7.1f, 14.3f), 0.3f);
	auto right_leg_sphere_14_instance_ptr = make_shared<instance>(right_leg_sphere_14_model, sphere_material);
	world.add(right_leg_sphere_14_instance_ptr);
	sphere* right_leg_sphere_15_model = new sphere(point3(21.9f, 7.6f, 14.4f), 0.3f);
	auto right_leg_sphere_15_instance_ptr = make_shared<instance>(right_leg_sphere_15_model, sphere_material);
	world.add(right_leg_sphere_15_instance_ptr);
	sphere* right_leg_sphere_16_model = new sphere(point3(22.0f, 8.1f, 14.5f), 0.3f);
	auto right_leg_sphere_16_instance_ptr = make_shared<instance>(right_leg_sphere_16_model, sphere_material);
	world.add(right_leg_sphere_16_instance_ptr);
	sphere* left_leg_sphere_1_model = new sphere(point3(21.0f, 0.0f, 15.5f), 0.1f);
	auto left_leg_sphere_1_instance_ptr = make_shared<instance>(left_leg_sphere_1_model, sphere_material);
	world.add(left_leg_sphere_1_instance_ptr);
	sphere* left_leg_sphere_2_model = new sphere(point3(21.0f, 0.3f, 15.5f), 0.2f);
	auto left_leg_sphere_2_instance_ptr = make_shared<instance>(left_leg_sphere_2_model, sphere_material);
	world.add(left_leg_sphere_2_instance_ptr);
	sphere* left_leg_sphere_3_model = new sphere(point3(20.9f, 0.8f, 15.4f), 0.3f);
	auto left_leg_sphere_3_instance_ptr = make_shared<instance>(left_leg_sphere_3_model, sphere_material);
	world.add(left_leg_sphere_3_instance_ptr);
	sphere* left_leg_sphere_4_model = new sphere(point3(20.8f, 1.4f, 15.3f), 0.3f);
	auto left_leg_sphere_4_instance_ptr = make_shared<instance>(left_leg_sphere_4_model, sphere_material);
	world.add(left_leg_sphere_4_instance_ptr);
	sphere* left_leg_sphere_5_model = new sphere(point3(20.7f, 2.0f, 15.2f), 0.3f);
	auto left_leg_sphere_5_instance_ptr = make_shared<instance>(left_leg_sphere_5_model, sphere_material);
	world.add(left_leg_sphere_5_instance_ptr);
	sphere* left_leg_sphere_6_model = new sphere(point3(20.6f, 2.6f, 15.1f), 0.3f);
	auto left_leg_sphere_6_instance_ptr = make_shared<instance>(left_leg_sphere_6_model, sphere_material);
	world.add(left_leg_sphere_6_instance_ptr);
	sphere* left_leg_sphere_7_model = new sphere(point3(20.5f, 3.2f, 15.0f), 0.3f);
	auto left_leg_sphere_7_instance_ptr = make_shared<instance>(left_leg_sphere_7_model, sphere_material);
	world.add(left_leg_sphere_7_instance_ptr);
	sphere* left_leg_sphere_8_model = new sphere(point3(20.4f, 3.8f, 14.9f), 0.3f);
	auto left_leg_sphere_8_instance_ptr = make_shared<instance>(left_leg_sphere_8_model, sphere_material);
	world.add(left_leg_sphere_8_instance_ptr);
	sphere* left_leg_sphere_9_model = new sphere(point3(20.3f, 4.5f, 14.8f), 0.4f);
	auto left_leg_sphere_9_instance_ptr = make_shared<instance>(left_leg_sphere_9_model, sphere_material);
	world.add(left_leg_sphere_9_instance_ptr);
	sphere* left_leg_sphere_10_model = new sphere(point3(20.4f, 5.1f, 14.9f), 0.3f);
	auto left_leg_sphere_10_instance_ptr = make_shared<instance>(left_leg_sphere_10_model, sphere_material);
	world.add(left_leg_sphere_10_instance_ptr);
	sphere* left_leg_sphere_11_model = new sphere(point3(20.5f, 5.6f, 15.0f), 0.3f);
	auto left_leg_sphere_11_instance_ptr = make_shared<instance>(left_leg_sphere_11_model, sphere_material);
	world.add(left_leg_sphere_11_instance_ptr);
	sphere* left_leg_sphere_12_model = new sphere(point3(20.6f, 6.1f, 15.1f), 0.3f);
	auto left_leg_sphere_12_instance_ptr = make_shared<instance>(left_leg_sphere_12_model, sphere_material);
	world.add(left_leg_sphere_12_instance_ptr);
	sphere* left_leg_sphere_13_model = new sphere(point3(20.7f, 6.6f, 15.2f), 0.3f);
	auto left_leg_sphere_13_instance_ptr = make_shared<instance>(left_leg_sphere_13_model, sphere_material);
	world.add(left_leg_sphere_13_instance_ptr);
	sphere* left_leg_sphere_14_model = new sphere(point3(20.8f, 7.1f, 15.3f), 0.3f);
	auto left_leg_sphere_14_instance_ptr = make_shared<instance>(left_leg_sphere_14_model, sphere_material);
	world.add(left_leg_sphere_14_instance_ptr);
	sphere* left_leg_sphere_15_model = new sphere(point3(20.9f, 7.6f, 15.4f), 0.3f);
	auto left_leg_sphere_15_instance_ptr = make_shared<instance>(left_leg_sphere_15_model, sphere_material);
	world.add(left_leg_sphere_15_instance_ptr);
	sphere* left_leg_sphere_16_model = new sphere(point3(21.0f, 8.1f, 15.5f), 0.3f);
	auto left_leg_sphere_16_instance_ptr = make_shared<instance>(left_leg_sphere_16_model, sphere_material);
	world.add(left_leg_sphere_16_instance_ptr);

	material* sphere_2_material = new material(color(1.0f, 0.0f, 0.0f), color(1.0f, 0.0f, 0.0f), color(1.0f, 0.0f, 0.0f), 0.5f);
	sphere_2_material->texture = new constant_texture(color(1.0f, 0.0f, 0.0f));
	// Chest
	sphere* chest_sphere_1_model = new sphere(point3(21.5f, 9.0f, 15.0f), 1.0f);
	auto chest_sphere_1_instance_ptr = make_shared<instance>(chest_sphere_1_model, sphere_2_material);
	world.add(chest_sphere_1_instance_ptr);
	sphere* chest_sphere_2_model = new sphere(point3(21.5f, 11.0f, 15.0f), 2.0f);
	auto chest_sphere_2_instance_ptr = make_shared<instance>(chest_sphere_2_model, sphere_2_material);
	world.add(chest_sphere_2_instance_ptr);

	// Neck
	sphere* neck_sphere_1_model = new sphere(point3(21.5f, 13.2f, 15.0f), 0.5f);
	auto neck_sphere_1_instance_ptr = make_shared<instance>(neck_sphere_1_model, sphere_material);
	world.add(neck_sphere_1_instance_ptr);

	// Head
	sphere* head_sphere_1_model = new sphere(point3(21.5f, 14.5f, 15.0f), 1.0f);
	auto head_sphere_1_instance_ptr = make_shared<instance>(head_sphere_1_model, sphere_material);
	world.add(head_sphere_1_instance_ptr);

	material* sphere_3_material = new material(color(0.0f, 0.0f, 1.0f), color(0.0f, 0.0f, 1.0f), color(0.0f, 0.0f, 1.0f), 0.5f);
	sphere_3_material->texture = new constant_texture(color(0.0f, 0.0f, 1.0f));
	// Eyes
	sphere* eyes_sphere_1_model = new sphere(point3(21.5f, 15.0f, 14.0f), 0.25f);
	auto eyes_sphere_1_instance_ptr = make_shared<instance>(eyes_sphere_1_model, sphere_3_material);
	world.add(eyes_sphere_1_instance_ptr);
	sphere* eyes_sphere_2_model = new sphere(point3(20.7f, 15.0f, 14.5f), 0.25f);
	auto eyes_sphere_2_instance_ptr = make_shared<instance>(eyes_sphere_2_model, sphere_3_material);
	world.add(eyes_sphere_2_instance_ptr);

	// Arms
	sphere* right_arm_sphere_1_model = new sphere(point3(19.5f, 12.0f, 15.5f), 0.3f);
	auto right_arm_sphere_1_instance_ptr = make_shared<instance>(right_arm_sphere_1_model, sphere_material);
	world.add(right_arm_sphere_1_instance_ptr);
	sphere* right_arm_sphere_2_model = new sphere(point3(19.2f, 11.5f, 15.5f), 0.3f);
	auto right_arm_sphere_2_instance_ptr = make_shared<instance>(right_arm_sphere_2_model, sphere_material);
	world.add(right_arm_sphere_2_instance_ptr);
	sphere* right_arm_sphere_3_model = new sphere(point3(18.9f, 11.0f, 15.5f), 0.3f);
	auto right_arm_sphere_3_instance_ptr = make_shared<instance>(right_arm_sphere_3_model, sphere_material);
	world.add(right_arm_sphere_3_instance_ptr);
	sphere* right_arm_sphere_4_model = new sphere(point3(18.6f, 10.6f, 15.5f), 0.3f);
	auto right_arm_4_instance_ptr = make_shared<instance>(right_arm_sphere_4_model, sphere_material);
	world.add(right_arm_4_instance_ptr);
	sphere* right_arm_sphere_5_model = new sphere(point3(18.3f, 10.1f, 15.5f), 0.3f);
	auto right_arm_sphere_5_instance_ptr = make_shared<instance>(right_arm_sphere_5_model, sphere_material);
	world.add(right_arm_sphere_5_instance_ptr);
	sphere* right_arm_sphere_6_model = new sphere(point3(18.0f, 9.6f, 15.5f), 0.3f);
	auto right_arm_sphere_6_instance_ptr = make_shared<instance>(right_arm_sphere_6_model, sphere_material);
	world.add(right_arm_sphere_6_instance_ptr);
	sphere* right_arm_sphere_7_model = new sphere(point3(17.7f, 9.1f, 15.5f), 0.3f);
	auto right_arm_sphere_7_instance_ptr = make_shared<instance>(right_arm_sphere_7_model, sphere_material);
	world.add(right_arm_sphere_7_instance_ptr);
	sphere* right_arm_sphere_8_model = new sphere(point3(17.4f, 8.6f, 15.5f), 0.4f);
	auto right_arm_sphere_8_instance_ptr = make_shared<instance>(right_arm_sphere_8_model, sphere_material);
	world.add(right_arm_sphere_8_instance_ptr);
	sphere* right_arm_sphere_9_model = new sphere(point3(17.4f, 8.6f, 14.9f), 0.3f);
	auto right_arm_sphere_9_instance_ptr = make_shared<instance>(right_arm_sphere_9_model, sphere_material);
	world.add(right_arm_sphere_9_instance_ptr);
	sphere* right_arm_sphere_10_model = new sphere(point3(17.4f, 8.6f, 14.3f), 0.3f);
	auto right_arm_sphere_10_instance_ptr = make_shared<instance>(right_arm_sphere_10_model, sphere_material);
	world.add(right_arm_sphere_10_instance_ptr);
	sphere* right_arm_sphere_11_model = new sphere(point3(17.4f, 8.6f, 13.7f), 0.3f);
	auto right_arm_sphere_11_instance_ptr = make_shared<instance>(right_arm_sphere_11_model, sphere_material);
	world.add(right_arm_sphere_11_instance_ptr);
	sphere* right_arm_sphere_12_model = new sphere(point3(17.4f, 8.6f, 13.1f), 0.3f);
	auto right_arm_sphere_12_instance_ptr = make_shared<instance>(right_arm_sphere_12_model, sphere_material);
	world.add(right_arm_sphere_12_instance_ptr);
	sphere* right_arm_sphere_13_model = new sphere(point3(17.4f, 8.6f, 12.5f), 0.3f);
	auto right_arm_sphere_13_instance_ptr = make_shared<instance>(right_arm_sphere_13_model, sphere_material);
	world.add(right_arm_sphere_13_instance_ptr);
	sphere* right_arm_sphere_14_model = new sphere(point3(17.4f, 8.6f, 11.9f), 0.3f);
	auto right_arm_sphere_14_instance_ptr = make_shared<instance>(right_arm_sphere_14_model, sphere_material);
	world.add(right_arm_sphere_14_instance_ptr);
	sphere* right_arm_sphere_15_model = new sphere(point3(17.4f, 8.6f, 11.3f), 0.3f);
	auto right_arm_sphere_15_instance_ptr = make_shared<instance>(right_arm_sphere_15_model, sphere_material);
	world.add(right_arm_sphere_15_instance_ptr);

	sphere* left_arm_sphere_1_model = new sphere(point3(23.5f, 12.0f, 15.5f), 0.3f);
	auto left_arm_sphere_1_instance_ptr = make_shared<instance>(left_arm_sphere_1_model, sphere_material);
	world.add(left_arm_sphere_1_instance_ptr);
	sphere* left_arm_sphere_2_model = new sphere(point3(23.8f, 11.4f, 15.5f), 0.3f);
	auto left_arm_sphere_2_instance_ptr = make_shared<instance>(left_arm_sphere_2_model, sphere_material);
	world.add(left_arm_sphere_2_instance_ptr);
	sphere* left_arm_sphere_3_model = new sphere(point3(24.1f, 10.8f, 15.5f), 0.3f);
	auto left_arm_sphere_3_instance_ptr = make_shared<instance>(left_arm_sphere_3_model, sphere_material);
	world.add(left_arm_sphere_3_instance_ptr);
	sphere* left_arm_sphere_4_model = new sphere(point3(24.4f, 10.2f, 15.5f), 0.3f);
	auto left_arm_sphere_4_instance_ptr = make_shared<instance>(left_arm_sphere_4_model, sphere_material);
	world.add(left_arm_sphere_4_instance_ptr);
	sphere* left_arm_sphere_5_model = new sphere(point3(24.7f, 9.6f, 15.5f), 0.3f);
	auto left_arm_sphere_5_instance_ptr = make_shared<instance>(left_arm_sphere_5_model, sphere_material);
	world.add(left_arm_sphere_5_instance_ptr);
	sphere* left_arm_sphere_6_model = new sphere(point3(25.0f, 9.0f, 15.5f), 0.3f);
	auto left_arm_sphere_6_instance_ptr = make_shared<instance>(left_arm_sphere_6_model, sphere_material);
	world.add(left_arm_sphere_6_instance_ptr);
	sphere* left_arm_sphere_7_model = new sphere(point3(25.3f, 8.4f, 15.5f), 0.3f);
	auto left_arm_sphere_7_instance_ptr = make_shared<instance>(left_arm_sphere_7_model, sphere_material);
	world.add(left_arm_sphere_7_instance_ptr);
	sphere* left_arm_sphere_8_model = new sphere(point3(25.6f, 7.8f, 15.5f), 0.4f);
	auto left_arm_sphere_8_instance_ptr = make_shared<instance>(left_arm_sphere_8_model, sphere_material);
	world.add(left_arm_sphere_8_instance_ptr);
	sphere* left_arm_sphere_9_model = new sphere(point3(25.3f, 7.8f, 14.9f), 0.3f);
	auto left_arm_sphere_9_instance_ptr = make_shared<instance>(left_arm_sphere_9_model, sphere_material);
	world.add(left_arm_sphere_9_instance_ptr);
	sphere* left_arm_sphere_10_model = new sphere(point3(25.3f, 7.8f, 14.3f), 0.3f);
	auto left_arm_sphere_10_instance_ptr = make_shared<instance>(left_arm_sphere_10_model, sphere_material);
	world.add(left_arm_sphere_10_instance_ptr);
	sphere* left_arm_sphere_11_model = new sphere(point3(25.3f, 7.8f, 13.7f), 0.3f);
	auto left_arm_sphere_11_instance_ptr = make_shared<instance>(left_arm_sphere_11_model, sphere_material);
	world.add(left_arm_sphere_11_instance_ptr);
	sphere* left_arm_sphere_12_model = new sphere(point3(25.3f, 7.8f, 13.1f), 0.3f);
	auto left_arm_sphere_12_instance_ptr = make_shared<instance>(left_arm_sphere_12_model, sphere_material);
	world.add(left_arm_sphere_12_instance_ptr);
	sphere* left_arm_sphere_13_model = new sphere(point3(25.3f, 7.8f, 12.5f), 0.3f);
	auto left_arm_sphere_13_instance_ptr = make_shared<instance>(left_arm_sphere_13_model, sphere_material);
	world.add(left_arm_sphere_13_instance_ptr);
	sphere* left_arm_sphere_14_model = new sphere(point3(25.3f, 7.8f, 11.9f), 0.3f);
	auto left_arm_sphere_14_instance_ptr = make_shared<instance>(left_arm_sphere_14_model, sphere_material);
	world.add(left_arm_sphere_14_instance_ptr);
	sphere* left_arm_sphere_15_model = new sphere(point3(25.3f, 7.8f, 11.3f), 0.3f);
	auto left_arm_sphere_15_instance_ptr = make_shared<instance>(left_arm_sphere_15_model, sphere_material);
	world.add(left_arm_sphere_15_instance_ptr);

	// Skittles 
	sphere* skittle_sphere_1_model = new sphere(point3(25.3f, 10.8f, 10.5f), 1.5f);
	auto skittle_sphere_1_instance_ptr = make_shared<instance>(skittle_sphere_1_model, sphere_3_material);
	world.add(skittle_sphere_1_instance_ptr);
	sphere* skittle_sphere_2_model = new sphere(point3(17.4f, 11.6f, 10.5f), 1.5f);
	auto skittle_sphere_2_instance_ptr = make_shared<instance>(skittle_sphere_2_model, sphere_3_material);
	world.add(skittle_sphere_2_instance_ptr);
	sphere* skittle_sphere_3_model = new sphere(point3(21.35f, 17.0f, 10.5f), 1.5f);
	auto skittle_sphere_3_instance_ptr = make_shared<instance>(skittle_sphere_3_model, sphere_3_material);
	world.add(skittle_sphere_3_instance_ptr);
}

int main(int argc, char* argv[]){
	// World
	hittable_list world;

	int num_samples = 256;
	int num_sets = 256;
	//Regular* sample_ptr = new Regular(num_samples);
	MultiJittered* sample_ptr = new MultiJittered(num_samples, num_sets);
	AmbientOccluder* ambient_occluder_ptr = new AmbientOccluder();
	ambient_occluder_ptr->set_sampler(sample_ptr);

	//build_test_scene(world);
	//build_Cornell_Box(world);
	//build_Compleat_Angler(world);
	build_The_Juggler(world);

	color lightgray = color(0.75f, 0.75f, 0.75f);
	point3 light_position(0.0f, 10.0f, 0.0f);
	point_light* worldlight = new point_light(light_position, lightgray, lightgray, lightgray);

	camera cam; 
	cam.lookfrom = point3(-30.0f, 15.0f, -30.0f);
	cam.lookat = point3(21.5f, 10.0f, 15.0f);

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
