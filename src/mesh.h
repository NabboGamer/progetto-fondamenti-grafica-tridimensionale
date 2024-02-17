#pragma once

#include <vector>
#include <cstdio>
#include <iostream>
#include "point2D.h"
#include "vec3.h"
#include "aabb.h"
#include "ray.h"
#include "object.h"
#include "hittable.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

using namespace std;

class mesh : public object {
public:

	vector<point3> 			vertices;				// mesh vertices 
	vector<vec3> 			normals;				// average normal at each vertex;
	vector<point2>				textures;
	vector<vector<int> > 		vertex_faces;			// the triangles shared by each vertex
	int num_vertices; 									// number of vertices
	int num_normals;
	int num_textures;
	int num_shapes;
	int num_faces;
	aabb aabb_mesh;

	bool areNormals = false;

	mesh(const char* filename, const char* basepath = NULL, bool triangulate = true) {
		load_mesh(filename, basepath, triangulate);
	};

	bool load_mesh(const char* filename, const char* basepath, bool triangulate);

	virtual bool hit(const ray& r, float tmin, float tmax, hit_record &rec) const;
	virtual bool hit_shadow(const ray& r, float t_min, float t_max) const;

	virtual bool bounding_box(aabb& box) const;
};

bool mesh::bounding_box(aabb& box) const {
	box = aabb_mesh;
	return true;
}

bool mesh::load_mesh(const char* filename, const char* basepath = NULL, bool triangulate = true)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string err;
	std::string war;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &war, &err, filename, basepath, triangulate);

	if (!err.empty()) {
		std::cerr << err << std::endl;
		return false;
	}

	if (!ret) {
		printf("Failed to load/parse .obj.\n");
		return false;
	}

	num_vertices = (attrib.vertices.size() / 3);
	num_normals = (attrib.normals.size() / 3);
	num_textures = (attrib.texcoords.size() / 2);
	num_shapes = shapes.size();

	aabb_mesh.x.min = aabb_mesh.y.min = aabb_mesh.z.min = FLT_MIN;
	aabb_mesh.x.max = aabb_mesh.y.max = aabb_mesh.z.max = FLT_MAX;;
	
	for (int v = 0; v < attrib.vertices.size() / 3; v++) {
		point3 p = point3(attrib.vertices[3 * v + 0], attrib.vertices[3 * v + 1], attrib.vertices[3 * v + 2]);

		aabb_mesh.x.min = ffmin(p[0], aabb_mesh.x.min);
		aabb_mesh.y.min = ffmin(p[1], aabb_mesh.y.min);
		aabb_mesh.z.min = ffmin(p[2], aabb_mesh.z.min);
		aabb_mesh.x.max = ffmax(p[0], aabb_mesh.x.max);
		aabb_mesh.y.max = ffmax(p[1], aabb_mesh.y.max);
		aabb_mesh.z.max = ffmax(p[2], aabb_mesh.z.max);

		vertices.push_back(p);
	}

	if (attrib.normals.size() > 0)
	{
		areNormals = true;
		for (int v = 0; v < attrib.normals.size() / 3; v++) {
			normals.push_back(vec3(attrib.normals[3 * v + 0], attrib.normals[3 * v + 1], attrib.normals[3 * v + 2]));
		}
	}
	
	if (attrib.texcoords.size() > 0)
	{
		for (int v = 0; v < attrib.texcoords.size() / 2; v++) {
			textures.push_back(point2(attrib.texcoords[2 * v + 0], attrib.texcoords[2 * v + 1]));
		}
	}

	vertex_faces.push_back(vector<int>());
	vertex_faces.push_back(vector<int>());
	vertex_faces.push_back(vector<int>());

	num_faces = 0;
	// For each shape
	for (int s = 0; s < shapes.size(); s++) {
		int index_offset = 0;

		// For each face
		for (int f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
			int fv = shapes[s].mesh.num_face_vertices[f];
			num_faces++;

			// For each vertex in the face
			for (int v = 0; v < fv; v++) {
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

				vertex_faces[0].push_back(idx.vertex_index);
				vertex_faces[1].push_back(idx.normal_index);
				vertex_faces[2].push_back(idx.texcoord_index);
			}

			index_offset += fv;
		}
	}

	return true;
}


bool triangle_intersection(const ray& r, float tmin, float tmax, hit_record &rec, const point3 &v0, const point3 &v1, const point3 &v2, float &u, float &v)
{
	const float EPSILON = 0.0000001f;
	vec3 edge1, edge2, h, s, q;
	float a, f;	// , u, v;
	edge1 = v1 - v0;
	edge2 = v2 - v0;
	h = cross(r.d, edge2);
	a = dot(edge1, h);
	if (a > -EPSILON && a < EPSILON)
		return false;
	f = 1 / a;
	s = r.o - v0;
	u = f * (dot(s, h));
	if (u < 0.0f || u > 1.0f)
		return false;
	q = cross(s, edge1);
	v = f * dot(r.d, q);
	if (v < 0.0f || u + v > 1.0f)
		return false;
	// At this stage we can compute t to find out where the intersection point is on the line.
	float t = f * dot(edge2, q);
	if (t > tmin && t < tmax) {
		if (t > EPSILON) // ray intersection
		{
			rec.normal = unit_vector(cross(v1 - v0, v2 - v0));
			rec.t = dot((v0 - r.o), rec.normal) / dot(r.direction(), rec.normal);
			rec.p = r.at(rec.t);
			return true;
		}
	}
	// This means that there is a line intersection but not a ray intersection.
	return false;
}


bool mesh::hit(const ray& ray, float t_min, float t_max, hit_record &rec) const
{
	bool hit_anything = false;
	hit_record temp_rec;
	float closest_so_far = t_max;
	float u, v;

	if (aabb_mesh.hit(ray, interval(t_min, t_max)) == false)
		return false;

	for (int i = 0; i < num_faces; i++)
	{
		int i0 = vertex_faces[0][3 * i + 0];
		int i1 = vertex_faces[0][3 * i + 1];
		int i2 = vertex_faces[0][3 * i + 2];

		point3 v0 = vertices[i0];
		point3 v1 = vertices[i1];
		point3 v2 = vertices[i2];

		if (triangle_intersection(ray, t_min, closest_so_far, temp_rec, v0, v1, v2, u, v)) {
			hit_anything = true;
			closest_so_far = temp_rec.t;
			rec = temp_rec;
			
			vec3 bary(1.0f - u - v, u, v);

			if (areNormals)
			{
				i0 = vertex_faces[1][3 * i + 0];
				i1 = vertex_faces[1][3 * i + 1];
				i2 = vertex_faces[1][3 * i + 2];
				
				rec.normal = unit_vector(normals[i0] * bary[0] + normals[i1] * bary[1] + normals[i2] * bary[2]);
			}

			i0 = vertex_faces[2][3 * i + 0];
			i1 = vertex_faces[2][3 * i + 1];
			i2 = vertex_faces[2][3 * i + 2];
			
			point2 uv0 = textures[i0];
			point2 uv1 = textures[i1];
			point2 uv2 = textures[i2];
			
			point2 uv = uv0 * bary[0] + uv1 * bary[1] + uv2 * bary[2];
			rec.u = uv.x;
			rec.v = uv.y;
		}
	}
	return hit_anything;
}

bool mesh::hit_shadow(const ray& ray, float t_min, float t_max) const
{
	hit_record temp_rec;
	float closest_so_far = t_max;
	float u, v;

	if (aabb_mesh.hit(ray, interval(t_min, t_max)) == false)
		return false;

	for (int i = 0; i < num_faces; i++)
	{
		int i0 = vertex_faces[0][3 * i + 0];
		int i1 = vertex_faces[0][3 * i + 1];
		int i2 = vertex_faces[0][3 * i + 2];

		point3 v0 = vertices[i0];
		point3 v1 = vertices[i1];
		point3 v2 = vertices[i2];

		if (triangle_intersection(ray, t_min, closest_so_far, temp_rec, v0, v1, v2, u, v))
			return true;
	}
	return false;
}
