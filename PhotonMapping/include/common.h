#pragma once
#include <random>
#include <time.h>
extern const float EPS; 
extern const float PI; 
extern std::default_random_engine e;
extern std::uniform_real_distribution<float> u;
extern const int maxEmitPhoton;
extern const int maxPhotonTracingDepth;
extern const int maxRayTracingDepth;

#define RandomRealZeroOne() (u(e))