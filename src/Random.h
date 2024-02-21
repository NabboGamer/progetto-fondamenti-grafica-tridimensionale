#include <cstdlib>

inline void set_rand_seed(const int seed) { srand(seed); }

inline int rand_int() { return rand(); }

inline float rand_float() { return static_cast<float>(rand()) / static_cast<float>(RAND_MAX); }

inline float rand_float(int l, float h) { return rand_float() * (h - l) + l; }

inline int rand_int(int l, int h) { return (int)(rand_float(0, (float)h - (float)l + 1.0f) + (float)l); }