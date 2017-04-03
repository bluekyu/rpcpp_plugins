#pragma once

#include <luse.h>

namespace rpplugins {

class Line3D
{
public:
    virtual void add_point(const LPoint3& point) = 0;
};

}
