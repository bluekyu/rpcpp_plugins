#pragma once

#include "../include/line_3d.hpp"

namespace rpplugins {

class Line3DImpl: Line3D
{
public:
    enum class ProcessingType
    {
        CPU = 0,
        GPU,
    };

public:
    Line3DImpl(const LPoint3& start_point, const LPoint3& end_point, const LVecBase3& normal);

    void add_point(const LPoint3& point) override;

private:
    std::vector<LPoint3> points_;
    ProcessingType processing_type_;
};

}
