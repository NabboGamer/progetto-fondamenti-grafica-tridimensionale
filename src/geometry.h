#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "object.h"
#include "math.h"
#include "vec3.h"
#include "material.h"
#include "hittable.h"
#include <assert.h>

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

class quadrilateral : public hittable {

public:

    quadrilateral(point3 _p_0, point3 _p_1, point3 _p_2, point3 _p_3) : p_0(_p_0), p_1(_p_1), p_2(_p_2), p_3(_p_3) {}

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        /*
        *   Per verificare se quattro punti nello spazio sono complanari, è possibile utilizzare il concetto di determinante.
        *   Quattro punti nello spazio sono complanari se e solo se il determinante della matrice avente come righe i vettori
        *   che collegano questi punti è uguale a zero.
        */
        if (!arePointsCoplanar(p_0, p_1, p_2, p_3)) {
            return false;
        }

        vec3 normal = cross(p_1 - p_0, p_3 - p_0);
        float t = (dot((p_0 - r.origin()), normal)) / (dot(r.direction(), normal));
        if (!ray_t.surrounds(t)) {
            return false;
        }
        point3 p = r.at(t);

        if (isPointInsideQuadrilateral(p_0, p_1, p_2, p_3, p)) {
            rec.t = t;
            rec.p = r.at(rec.t);
            vec3 outward_normal = normal;
            rec.set_face_normal(r, outward_normal);

            barycentricCoordinates(rec.p, rec.u, rec.v);
            mapToUnitSquare(rec.u, rec.v);
            
            return true;
        }
    }

    bool hit_shadow(const ray& r, interval ray_t) const override {
        if (!arePointsCoplanar(p_0, p_1, p_2, p_3)) {
            return false;
        }

        vec3 normal = cross(p_1 - p_0, p_3 - p_0);
        float t = (dot((p_0 - r.origin()), normal)) / (dot(r.direction(), normal));
        if (!ray_t.surrounds(t)) {
            return false;
        }
        point3 p = r.at(t);

        if (isPointInsideQuadrilateral(p_0, p_1, p_2, p_3, p)) {
            return true;
        }
    }

    bool isPointInsideQuadrilateral(const point3& A, const point3& B, const point3& C, const point3& D, const point3& P) const {
        // Funzione per verificare se un punto P giace all'interno di un quadrilatero ABCD
        point3 crossProduct1 = crossProduct(A, B, P);
        point3 crossProduct2 = crossProduct(B, C, P);
        point3 crossProduct3 = crossProduct(C, D, P);
        point3 crossProduct4 = crossProduct(D, A, P);

        // Verifica l'orientamento dei prodotti vettoriali
        // Questa tecnica sfrutta la proprietà che i punti all'interno di un poligono composto da vertici disposti in senso 
        // antiorario (o orario) hanno prodotti vettoriali concordi rispetto ai bordi del poligono
        bool isInside = (crossProduct1.e[0] * crossProduct2.e[0] >= 0) &&
                        (crossProduct1.e[1] * crossProduct2.e[1] >= 0) &&
                        (crossProduct1.e[2] * crossProduct2.e[2] >= 0) &&
                        (crossProduct2.e[0] * crossProduct3.e[0] >= 0) &&
                        (crossProduct2.e[1] * crossProduct3.e[1] >= 0) &&
                        (crossProduct2.e[2] * crossProduct3.e[2] >= 0) &&
                        (crossProduct3.e[0] * crossProduct4.e[0] >= 0) &&
                        (crossProduct3.e[1] * crossProduct4.e[1] >= 0) &&
                        (crossProduct3.e[2] * crossProduct4.e[2] >= 0) &&
                        (crossProduct4.e[0] * crossProduct1.e[0] >= 0) &&
                        (crossProduct4.e[1] * crossProduct1.e[1] >= 0) &&
                        (crossProduct4.e[2] * crossProduct1.e[2] >= 0);

        return isInside;
    }

    // Funzione per calcolare il prodotto vettoriale tra due vettori AB e AC
    point3 crossProduct(const point3& A, const point3& B, const point3& C) const {
        point3 AB = { B.e[0] - A.e[0], B.e[1] - A.e[1], B.e[2] - A.e[2] };
        point3 AC = { C.e[0] - A.e[0], C.e[1] - A.e[1], C.e[2] - A.e[2] };

        point3 result;
        result.e[0] = AB.e[1] * AC.e[2] - AB.e[2] * AC.e[1];
        result.e[1] = AB.e[2] * AC.e[0] - AB.e[0] * AC.e[2];
        result.e[2] = AB.e[0] * AC.e[1] - AB.e[1] * AC.e[0];

        return result;
    }

    double determinant(const point3& p1, const point3& p2, const point3& p3) const {
        return p1.e[0] * (p2.e[1] * p3.e[2] - p3.e[1] * p2.e[2]) -
            p2.e[0] * (p1.e[1] * p3.e[2] - p3.e[1] * p1.e[2]) +
            p3.e[0] * (p1.e[1] * p2.e[2] - p2.e[1] * p1.e[2]);
    }

    // Funzione per verificare la complanarità di quattro punti
    bool arePointsCoplanar(const point3& p1, const point3& p2, const point3& p3, const point3& p4) const{
        // Calcola il determinante della matrice formata dai vettori che collegano i punti
        double det = determinant(p2 - p1, p3 - p1, p4 - p1);

        // Verifica se il determinante è vicino a zero (considerando una tolleranza)
        const double tolerance = 1e-10;
        return std::abs(det) < tolerance;
    }

    void barycentricCoordinates(const point3& point, float& u, float& v) const {
        // Calcoliamo i vettori che definiscono il quadrilatero
        point3 e0 = p_1 - p_0;
        point3 e1 = p_3 - p_0;
        point3 e2 = point - p_0;

        // Calcoliamo i prodotti scalari
        float dot00 = dot(e0, e0);
        float dot01 = dot(e0, e1);
        float dot02 = dot(e0, e2);
        float dot11 = dot(e1, e1);
        float dot12 = dot(e1, e2);

        // Calcoliamo la matrice inversa
        float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
        u = (dot11 * dot02 - dot01 * dot12) * invDenom;
        v = (dot00 * dot12 - dot01 * dot02) * invDenom;
    }

    // Funzione per mappare le coordinate baricentriche in [-1, 1]
    void mapToUnitSquare(float& u, float& v) const {
        u = 2.0f * u - 1.0f;
        v = 2.0f * v - 1.0f;
    }

private:
    point3 p_0;
    point3 p_1;
    point3 p_2;
    point3 p_3;
};

class box : public hittable {

public:

    box(const point3& min, const point3& max) {
        assert(min < max);
        parameters[0] = min;
        parameters[1] = max;
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        /*
        *   Intersezione Raggio - Parallelelpipedo basata su :
        *   Amy Williams, Steve Barrus, R. Keith Morley, and Peter Shirley
        *   "An Efficient and Robust Ray-Box Intersection Algorithm"
        *   Journal of graphics tools 2005
        */
        float tmin, tmax, tymin, tymax, tzmin, tzmax;

        /*std::cout << "Posizione pmin cubo: " << "(" << parameters[0].x() << "," << parameters[0].y() << "," << parameters[0].z() << ")" << std::endl;
        std::cout << "Posizione pmax cubo: " << "(" << parameters[1].x() << "," << parameters[1].y() << "," << parameters[1].z() << ")" << std::endl;
        std::cout << "Direzione inversa raggio: " << "(" << r.inv_direction.x() << "," << r.inv_direction.y() << "," << r.inv_direction.z() << ")" << std::endl;
        std::cout << "Segni direzione inversa raggio: " << r.sign[0] << " " << r.sign[0] << " " << r.sign[0] << std::endl;*/
        tmin = (parameters[r.sign[0]].e[0] - r.o.e[0]) * r.inv_direction.e[0];
        tmax = (parameters[1 - r.sign[0]].e[0] - r.o.e[0]) * r.inv_direction.e[0];
        tymin = (parameters[r.sign[1]].e[1] - r.o.e[1]) * r.inv_direction.e[1];
        tymax = (parameters[1 - r.sign[1]].e[1] - r.o.e[1]) * r.inv_direction.e[1];
        if ((tmin > tymax) || (tymin > tmax))
            return false;
        if (tymin > tmin)
            tmin = tymin;
        if (tymax < tmax)
            tmax = tymax;
        tzmin = (parameters[r.sign[2]].e[2] - r.o.e[2]) * r.inv_direction.e[2];
        tzmax = (parameters[1 - r.sign[2]].e[2] - r.o.e[2]) * r.inv_direction.e[2];
        if ((tmin > tzmax) || (tzmin > tmax))
            return false;
        if (tzmin > tmin)
            tmin = tzmin;
        if (tzmax < tmax)
            tmax = tzmax;
        if ((tmin < ray_t.max) && (tmax > ray_t.min)) {
            rec.t = tmin;
            rec.p = r.at(rec.t);
            vec3 outward_normal = calculate_normal(rec.p, parameters[0], parameters[1]);
            rec.set_face_normal(r, outward_normal);
            return true;
        }
        else {
            return false;
        }
    }

    bool hit_shadow(const ray& r, interval ray_t) const override {
        float tmin, tmax, tymin, tymax, tzmin, tzmax;

        tmin = (parameters[r.sign[0]].e[0] - r.o.e[0]) * r.inv_direction.e[0];
        tmax = (parameters[1 - r.sign[0]].e[0] - r.o.e[0]) * r.inv_direction.e[0];
        tymin = (parameters[r.sign[1]].e[1] - r.o.e[1]) * r.inv_direction.e[1];
        tymax = (parameters[1 - r.sign[1]].e[1] - r.o.e[1]) * r.inv_direction.e[1];
        if ((tmin > tymax) || (tymin > tmax))
            return false;
        if (tymin > tmin)
            tmin = tymin;
        if (tymax < tmax)
            tmax = tymax;
        tzmin = (parameters[r.sign[2]].e[2] - r.o.e[2]) * r.inv_direction.e[2];
        tzmax = (parameters[1 - r.sign[2]].e[2] - r.o.e[2]) * r.inv_direction.e[2];
        if ((tmin > tzmax) || (tzmin > tmax))
            return false;
        if (tzmin > tmin)
            tmin = tzmin;
        if (tzmax < tmax)
            tmax = tzmax;
        if ((tmin < ray_t.max) && (tmax > ray_t.min)) {
            return true;
        }
        else {
            return false;
        }
    }

    vec3 calculate_normal(point3 p_intersection, const point3 p_min, const point3 p_max) const {
        /*
        *   Il calcolo della normale è un problema non banale(e non risolto nel paper) per il quale viene fatto un confronto
        *   delle componenti del p.to di intersezione prima con le componenti del p_min e poi se necessario del p_max.
        *   Alla base di questo metodo vi è il fatto che un piano nello spazio avrà sempre una delle tre componenti costanti
        *   se esso è non obliquo. Siccome la classe rappresenta parallelepipedi non obliqui (ovvero box) tutte le 6 facce
        *   del parallelepipedo avranno una componente costante. Questa componente viene trovata confrontando il p.to di
        *   intersezione con p_min e p_max e così si è anche individuato indirettamente il piano. Dalla conoscenza del piano
        *   viene costruita una normale a esso con base nel punto di intersezione.
        *
        *   Il confronto con la epsilon è necessario quando si devono uguagliare dei float.
        */
        const float epsilon = 1e-4;
        if (std::abs(p_intersection.e[0] - p_min.e[0]) < epsilon) {
            vec3 normal = point3(p_intersection.e[0] - 1, p_intersection.e[1], p_intersection.e[2]) - p_intersection;
            normal = unit_vector(normal);
            return normal;
        }
        if (std::abs(p_intersection.e[1] - p_min.e[1]) < epsilon) {
            vec3 normal = point3(p_intersection.e[0], p_intersection.e[1] - 1, p_intersection.e[2]) - p_intersection;
            normal = unit_vector(normal);
            return normal;
        }
        if (std::abs(p_intersection.e[2] - p_min.e[2]) < epsilon) {
            vec3 normal = point3(p_intersection.e[0], p_intersection.e[1], p_intersection.e[2] - 1) - p_intersection;
            normal = unit_vector(normal);
            return normal;
        }

        if (std::abs(p_intersection.e[0] - p_max.e[0]) < epsilon) {
            vec3 normal = point3(p_intersection.e[0] + 1, p_intersection.e[1], p_intersection.e[2]) - p_intersection;
            normal = unit_vector(normal);
            return normal;
        }
        if (std::abs(p_intersection.e[1] - p_max.e[1]) < epsilon) {
            vec3 normal = point3(p_intersection.e[0], p_intersection.e[1] + 1, p_intersection.e[2]) - p_intersection;
            normal = unit_vector(normal);
            return normal;
        }
        if (std::abs(p_intersection.e[2] - p_max.e[2]) < epsilon) {
            vec3 normal = point3(p_intersection.e[0], p_intersection.e[1], p_intersection.e[2] + 1) - p_intersection;
            normal = unit_vector(normal);
            return normal;
        }
    }

private:
    point3 parameters[2];
};

#endif
