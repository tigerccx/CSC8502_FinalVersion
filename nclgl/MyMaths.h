#pragma once

#include <cstdlib>
#include "minmax.h"

using namespace std;

#define EPSINON_FLOAT 0.000001f

class MyMaths {
public:
	static inline float RandomFloat01() {
		return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	}
	static inline float RandomFloatN1P1() {
		return RandomFloat01() * 2.0f - 1.0f;
	}

	static inline int Sign(float x) { if (x > 0)return 1; else if (x == 0) return 0; else return -1; }
	static inline int SignNoZero(float x) { if (x >= 0)return 1; else return -1; }

	static inline bool EpsEqual(float x, float y, float eps = EPSINON_FLOAT) { float diff = abs(x - y); return diff < eps; }

	static inline float Clamp(float x, float low, float high) { return max(min(x, high), low); }

	static float Lerp(float from, float to, float f) {
		return (to - from) * f + from;
	}
};