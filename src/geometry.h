#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "object.h"
#include "math.h"
#include "vec3.h"
#include "material.h"
#include "hittable.h"

class plane : public hittable {

public:

	plane() {
		p_0 = point3(0.0f, 0.0f, 0.0f);
		normal = vec3(0.0f, 1.0f, 0.0f);
	}

	plane(point3 _p_0, vec3 _normal) : p_0(_p_0), normal(_normal) {}

	bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
		float t = (dot((p_0 - r.origin()), normal)) / (dot(r.direction(), normal));

		if (!ray_t.surrounds(t)) {
			return false;
		}

		rec.t = t;
		rec.p = r.at(rec.t);
		vec3 outward_normal = normal;
		rec.set_face_normal(r, outward_normal);

		return true;
	}

	bool hit_shadow(const ray& r, interval ray_t) const override {
		float t = (dot((p_0 - r.origin()), normal)) / (dot(r.direction(), normal));

		if (!ray_t.surrounds(t)) {
			return false;
		}

		return true;
	}

private:
	point3 p_0;
	vec3 normal;
};

class sphere : public hittable {
public:
    sphere() {
        center = point3(0.0f, 0.0f, 0.0f);
        radius = 1.0f;
    }
    sphere(point3 _center, float _radius) : center(_center), radius(_radius) {}

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        vec3 oc = r.origin() - center;
        float a = r.direction().length_squared();
        float half_b = dot(oc, r.direction());
        float c = oc.length_squared() - radius * radius;

        float discriminant = half_b * half_b - a * c;
        if (discriminant < 0) return false;
        float sqrtd = sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        float root = (-half_b - sqrtd) / a;
        if (!ray_t.surrounds(root)) {
            root = (-half_b + sqrtd) / a;
            if (!ray_t.surrounds(root))
                return false;
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        vec3 outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);

        return true;
    }

    bool hit_shadow(const ray& r, interval ray_t) const override {
        vec3 oc = r.origin() - center;
        float a = r.direction().length_squared();
        float half_b = dot(oc, r.direction());
        float c = oc.length_squared() - radius * radius;

        float discriminant = half_b * half_b - a * c;
        if (discriminant < 0) return false;
        float sqrtd = sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        float root = (-half_b - sqrtd) / a;
        if (!ray_t.surrounds(root)) {
            root = (-half_b + sqrtd) / a;
            if (!ray_t.surrounds(root))
                return false;
        }

        return true;
    }

private:
    point3 center;
    float radius;
};

#endif
