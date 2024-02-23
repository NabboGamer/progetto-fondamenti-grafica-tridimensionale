#include "Regular.h"

Regular::Regular(const int num) : Sampler(num) { generate_samples(); }

void Regular::generate_samples() {
    int n = (int)sqrt((float)num_samples);

    for (int j = 0; j < num_sets; j++) {
        for (int p = 0; p < n; p++) {
            for (int q = 0; q < n; q++) {
                samples.push_back(point3(((float)q + 0.5f) / (float)n, ((float)p + 0.5f) / (float)n, 0.0f));
            }
        }
    }
}
