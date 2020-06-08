#pragma once

#include <random>

#include "nray.h"

// This is our Random-Number-Generator class helper
class Rng {
  public:
    static Float Rand01() {
        return  distribution01(generator);
    }

    static Float RandRange(Float min, Float max) {
      return Rand01() * (max-min) + min;
    }

    static std::uniform_real_distribution<Float> distribution01;
    static std::mt19937 generator;
};
