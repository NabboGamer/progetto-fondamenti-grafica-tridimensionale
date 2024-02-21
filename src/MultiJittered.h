#include "Sampler.h"

class MultiJittered : public Sampler {
public:

    MultiJittered() = default;

    MultiJittered(const int num_samples);

    MultiJittered(const int num_samples, const int m);

private:

    virtual void generate_samples() override;
};
