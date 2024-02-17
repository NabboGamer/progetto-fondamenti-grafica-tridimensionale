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

inline vec3 reflect(const vec3& i, const vec3& n)
{
	//return i - 2.0f * dot(i, n) * n;
	return  2.0f * dot(i, n) * n - i; // i vettore uscente dalla normale
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
