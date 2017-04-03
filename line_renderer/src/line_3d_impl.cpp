#include "line_3d_impl.hpp"

namespace rpplugins {

Line3DImpl::Line3DImpl(const LPoint3& start_point, const LPoint3& end_point, const LVecBase3& normal)
{
    processing_type_ = ProcessingType::CPU;

    points_.push_back(start_point);
    points_.push_back(end_point);
}

void Line3DImpl::add_point(const LPoint3& point)
{
    points_.push_back(point);
}

}
