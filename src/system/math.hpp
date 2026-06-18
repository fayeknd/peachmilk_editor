#pragma once
#include <cmath>
#include <stdio.h>
#include <iostream>
#include "../headers.h"

class Math {
public:

    inline static const glm::vec3 up() { return glm::vec3(0, 1, 0); }
    inline static const glm::vec3 right() { return glm::vec3(1, 0, 0); }
    inline static const glm::vec3 forward() { return glm::vec3(0, 0, 1); }

    inline static const glm::vec3 down() { return -up(); }
    inline static const glm::vec3 left() { return -right(); }
    inline static const glm::vec3 back() { return -forward(); }

    inline static const double pi = 3.141592653589793;
    inline static const double tau = 6.283185307179586;
    inline static const double goldenRatio = (glm::sqrt(5) - 1) / 2;
    static double Inifinity() { return std::numeric_limits<double>::infinity(); }

    static bool isEven(int val) {
        return val % 2 == 0;
    }

    static int firstDigit(int n)
    {
        while (n >= 10) 
            n /= 10;

        return n;
    }

    template <typename T>
	static T clamp(T v, T min, T max) {
		v = (v > min) ? v : min;
		v = (v < max) ? v : max;
		return v;
	}

    static double abs(double x) {
		if (x < 0)
			return -1 * x;
		return x;
	}

    static double deg2rad(double degrees) {
		return degrees * (pi / (double)180);
	}
    static double rad2deg(double radians) {
		return radians * ((double)180 / pi);
	}

};