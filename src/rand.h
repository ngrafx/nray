
#pragma once

#include <random>

#include "nray.h"


class Rng {
  public:
    static Float Rand01() {
        return  distribution01(generator);
    }

    static Float RandRange(Float min, Float max) {
      return Rand01() * (max-min) + min;
    }

    static Vec3 RandomVector() {
      Float a = RandRange(0, 2*Pi);
      Float z = RandRange(-1, 1);
      Float r = sqrt(1 - z*z);
      return Vec3(r*cos(a), r*sin(a), z);
    }

    static std::uniform_real_distribution<Float> distribution01;
    static std::mt19937 generator;
};