#pragma once

#include <functional>

#include "geometry/named_quantities.hpp"

namespace principia {

using geometry::Instant;
using geometry::Position;
using geometry::Rotation;

namespace physics {

// An orthonormal basis for the tangent space to the manifold of |Position|s at
// time |Instant|, described in the coordinates of the global spacetime chart
// |Frame| as a rotation of the standard basis.
template<typename Frame>
using FrameField =
    std::function<Rotation<Frame, Frame>(Position<Frame>, Instant)>;

template<typename Frame>
FrameField<Frame> CoordinateFrame();

}  // namespace physics
}  // namespace principia

#include "physics/frame_field_body.hpp"
