#ifndef __REGULAR__
#define __REGULAR__

#include "Sampler.h"

class Regular : public Sampler {
public:

    Regular() = delete;

    Regular(const int num);

private:

    virtual void generate_samples() override;
};

#endif
