#ifndef __SAMPLER__
#define __SAMPLER__

#include <math.h>
#include <vector>
#include "vec3.h"

class Sampler {
public:

    Sampler();

    Sampler(const int num);

    Sampler(const int num, const int num_sets);

    void set_num_sets(const int np);

    virtual void generate_samples() = 0; // generate sample patterns in a unit square

    int get_num_samples();

    void shuffle_x_coordinates();

    void shuffle_y_coordinates();

    void setup_shuffled_indices();

    void map_samples_to_hemisphere(const float p);

    point3 sample_hemisphere(); // get next sample on unit hemisphere

    point3 sample_unit_square(); // get next sample on unit square

public:

    int num_samples;                          // the number of sample points in a set
    int num_sets;                             // the number of sample sets
    std::vector<point3> samples;              // sample points on a unit square
    std::vector<int> shuffled_indices;        // shuffled samples array indices
    std::vector<point3> hemisphere_samples;   // sample points on a unit hemisphere
    unsigned long count;                      // the current number of sample points used
    int jump;                                 // random index jump
};

#endif