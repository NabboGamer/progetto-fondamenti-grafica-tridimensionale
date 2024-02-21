#pragma once
#include <iostream>
#include <time.h>
#include <algorithm>
#include "camera.h"
#include "light.h"
#include "material.h"
#include "hittable.h"
#include "color.h"
#include "vec3.h"
#include "math.h"
#include "Sampler.h"

inline vec3 reflect(const vec3& i, const vec3& n){
	return  2.0f * dot(i, n) * n - i; // il vettore uscente dalla normale
}

inline bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted) {
	vec3 d = unit_vector(v);
	float dn = dot(d, n);
	float discriminant = 1.0f - ni_over_nt * ni_over_nt * (1.0f - dn * dn);
	if (discriminant > 0.0f) {
		refracted = ni_over_nt * (d - n * dn) - n * sqrt(discriminant);
		return true;
	}
	else
		return false;
}

color phong_shading(point_light& light, hit_record& hr, point3& camera_pos) {
	color ambient(0.0f, 0.0f, 0.0f);
	color diffuse(0.0f, 0.0f, 0.0f);
	color specular(0.0f, 0.0f, 0.0f);

	ambient = hr.m->ka * light.ambient;

	vec3 L = unit_vector(light.position - hr.p);
	float LDotN = max(dot(L, hr.normal), 0.0f);

	if (LDotN > 0.0f) {
		diffuse = hr.m->kd * light.diffuse * LDotN;

		vec3 R = reflect(L, hr.normal);

		vec3 V = unit_vector(camera_pos - hr.p);
		float VDotR = std::pow(max(dot(V, R), 0.0f), hr.m->alpha);

		specular = hr.m->ks * light.specular * VDotR;

		return ambient + diffuse + specular;
	}
	else
		return ambient;
}

color ambient_shading(point_light& light, hit_record& hr) {
	color ambient(0.0f, 0.0f, 0.0f);
	color diffuse(0.0f, 0.0f, 0.0f);
	color specular(0.0f, 0.0f, 0.0f);

	ambient = hr.m->ka * light.ambient;

	return ambient;
}

class AmbientOccluder{

public:

	AmbientOccluder() {}


	void set_sampler(Sampler* s_ptr) {
		if (sampler_ptr) {
			delete sampler_ptr;
			sampler_ptr = nullptr;
		}

		sampler_ptr = s_ptr;
		sampler_ptr->map_samples_to_hemisphere(1);
	}

	vec3 get_direction() {
		point3 sp = sampler_ptr->sample_hemisphere();
		return sp.x() * u + sp.y() * v + sp.z() * w;
	}

	bool in_shadow(const ray& ray, const hittable_list world) const {
		int num_objects = world.objects.size();

		for (int j = 0; j < num_objects; j++) {
			if (world.objects[j]->hit_shadow(ray, interval(0.0f, infinity))) {
				return true;
			}
		}

		return false;
	}

	color compute_color(const hittable_list world, hit_record& hr) {
		vec3 w = hr.normal;
		// Il vettore 'up' è leggermente jittered(ovvero gli viene agggiunto del rumore, è agitato letteralmente)
		// poichè nel caso in cui la normale sia verticale questo aiuta a ottenere un effetto maggiormente realistico
		vec3 v = cross(w, vec3(0.0072, 1.0f, 0.0034));
		v = unit_vector(v);
		vec3 u = cross(v, w);

		ray shadow_ray;
		shadow_ray.o = hr.p;
		shadow_ray.d = get_direction();

		if (in_shadow(shadow_ray, world)) {
			return min_amount * ls * _color;
		} else {
			return ls * _color;
		}
	}

private:

	float ls = 1.0f;
	color _color = color(1.0f, 1.0f, 1.0f);
	float min_amount = 0.0f;
	vec3 u = vec3(1.0f, 0.0f, 0.0f);
	vec3 v = vec3(0.0f, 1.0f, 0.0f);
	vec3 w = vec3(0.0f, 0.0f, 1.0f);
	Sampler* sampler_ptr = nullptr;

};
