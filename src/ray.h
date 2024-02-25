#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class ray {
public:

	point3 o;
	vec3 d;
	vec3 inv_direction;
	int sign[3];

	ray() {}

	ray(const point3& _origin, const vec3& _direction) { 
		o = _origin; 
		d = _direction;
		inv_direction = vec3(1 / d.e[0], 1 / d.e[1], 1 / d.e[2]);
		sign[0] = (inv_direction.e[0] < 0);
		sign[1] = (inv_direction.e[1] < 0);
		sign[2] = (inv_direction.e[2] < 0);
	}

	point3 origin() const { 
		return o; 
	}

	vec3 direction() const { 
		return d; 
	}

	void set_origin(point3 _origin) {
		o = _origin;
	}

	void set_direction(vec3 _direction) {
		d = _direction;
		inv_direction = vec3(1 / d.x(), 1 / d.y(), 1 / d.z());
		sign[0] = (inv_direction.x() < 0);
		sign[1] = (inv_direction.y() < 0);
		sign[2] = (inv_direction.z() < 0);
	}

	point3 at(float t) const { 
		return o + t * d; 
	}

	float t_at_point(point3& p) const { 
		return (p - o).length(); 
	}
};

#endif
