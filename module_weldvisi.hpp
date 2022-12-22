#pragma once
#include "flecs.h"

namespace simple {

struct Position {
    double x, y;
};

struct Velocity {
    double x, y;
};

struct module {
    module(flecs::world& world); // Ctor that loads the module
};

}




