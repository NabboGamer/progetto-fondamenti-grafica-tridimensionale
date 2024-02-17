#pragma once
#include "color.h"
#include "light.h"
#include "common.h"
#include "texture.h"

class material {
public:
	color ka;
	color kd;
	color ks;
	float alpha;

	material(color ambient, color diffuse, color specular, float a) : ka(ambient), kd(diffuse), ks(specular), alpha(a) {};

	material() {

		ka[0] = random_float();
		ka[1] = random_float();
		ka[2] = random_float();

		kd[0] = random_float();
		kd[1] = random_float();
		kd[2] = random_float();

		ks[0] = random_float();
		ks[1] = random_float();
		ks[2] = random_float();

		alpha = 20.0f + random_float() * 200.0f;
	}

	texture* texture;

	float reflective = 0.0;
	float refractive = 0.0;
};



	
