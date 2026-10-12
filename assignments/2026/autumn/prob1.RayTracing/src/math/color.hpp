#pragma once

#include <iosfwd>

#include "math/vec3.hpp"

double linear_to_gamma(double linear_component);

void write_color(std::ostream &out, color pixel_color);
