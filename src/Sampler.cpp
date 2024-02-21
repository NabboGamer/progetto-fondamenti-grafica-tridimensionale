#include "Sampler.h"

#include <algorithm>  // for random_shuffle in Sampler::setup_shuffled_indices
#include <iostream>
#include <random>
#include "Random.h"
#include "common.h"

Sampler::Sampler() : num_samples(1), num_sets(83), count(0), jump(0) {
    samples.reserve(num_samples * num_sets);
    setup_shuffled_indices();
}

Sampler::Sampler(const int ns) : num_samples(ns), num_sets(83), count(0), jump(0) {
    samples.reserve(num_samples * num_sets);
    setup_shuffled_indices();
}

Sampler::Sampler(const int ns, const int n_sets) : num_samples(ns), num_sets(n_sets), count(0), jump(0) {
    samples.reserve(num_samples * num_sets);
    setup_shuffled_indices();
}

void Sampler::set_num_sets(const int np) { num_sets = np; }

int Sampler::get_num_samples() { return num_samples; }

// shuffle the x coordinates of the points within each set

void Sampler::shuffle_x_coordinates() {
    for (int p = 0; p < num_sets; p++) {
        for (int i = 0; i < num_samples - 1; i++) {
            int target = rand_int() % num_samples + p * num_samples;
            float temp = samples[i + p * num_samples + 1].e[0];
            samples[i + p * num_samples + 1].e[0] = samples[target].e[0];
            samples[target].e[0] = temp;
        }
    }
}

// shuffle the y coordinates of the points within set

void Sampler::shuffle_y_coordinates() {
    for (int p = 0; p < num_sets; p++) {
        for (int i = 0; i < num_samples - 1; i++) {
            int target = rand_int() % num_samples + p * num_samples;
            float temp = samples[i + p * num_samples + 1].e[1];
            samples[i + p * num_samples + 1].e[1] = samples[target].e[1];
            samples[target].e[1] = temp;
        }
    }
}

// sets up randomly shuffled indices for the samples array

void Sampler::setup_shuffled_indices() {
    shuffled_indices.reserve(num_samples * num_sets);
    std::vector<int> indices;
    std::random_device rd;
    std::mt19937 g(rd());

    for (int j = 0; j < num_samples; j++) {
        indices.push_back(j);
    }

    for (int p = 0; p < num_sets; p++) {
        std::shuffle(indices.begin(), indices.end(), g);

        for (int j = 0; j < num_samples; j++) {
            shuffled_indices.push_back(indices[j]);
        }
    }
}

// Maps the 2D sample points to 3D points on a unit hemisphere with a cosine power
// density distribution in the polar angle

void Sampler::map_samples_to_hemisphere(const float exp) {
    size_t size = samples.size();
    hemisphere_samples.reserve(num_samples * num_sets);

    for (int j = 0; j < size; j++) {
        float cos_phi = cos(2.0 * PI * samples[j].e[0]);
        float sin_phi = sin(2.0 * PI * samples[j].e[0]);
        float cos_theta = pow((1.0f - samples[j].e[1]), 1.0f / (exp + 1.0f));
        float sin_theta = sqrt(1.0f - cos_theta * cos_theta);
        float pu = sin_theta * cos_phi;
        float pv = sin_theta * sin_phi;
        float pw = cos_theta;
        hemisphere_samples.push_back(point3((float)pu, (float)pv, (float)pw));
    }
}

point3 Sampler::sample_hemisphere() {
    if (count % num_samples == 0) {  // start of a new pixel
        jump = (rand_int() % num_sets) * num_samples;
    }

    return hemisphere_samples[jump + shuffled_indices[jump + count++ % num_samples]];
}
