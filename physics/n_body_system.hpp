﻿#pragma once

#include <memory>
#include <set>
#include <vector>

#include "geometry/named_quantities.hpp"
#include "integrators/symplectic_integrator.hpp"
#include "physics/body.hpp"
#include "physics/trajectory.hpp"
#include "quantities/quantities.hpp"

using principia::geometry::Instant;
using principia::integrators::SymplecticIntegrator;
using principia::quantities::Acceleration;
using principia::quantities::Length;
using principia::quantities::Speed;
using principia::quantities::Time;

namespace principia {
namespace physics {

template<typename InertialFrame>
class NBodySystem {
 public:
  using Trajectories = std::vector<Trajectory<InertialFrame>*>;  // Not owned.

  NBodySystem() = default;
  ~NBodySystem() = default;

  // The |integrator| must already have been initialized.  All the
  // |trajectories| must have the same |last_time()| and must be for distinct
  // bodies.
  void Integrate(SymplecticIntegrator<Length, Speed> const& integrator,
                 Instant const& tmax,
                 Time const& Δt,
                 int const sampling_period,
                 bool const tmax_is_exact,
                 Trajectories const& trajectories);

 private:
  using ReadonlyTrajectories = std::vector<Trajectory<InertialFrame> const*>;

  // No transfer of ownership.
  static void ComputeGravitationalAccelerations(
      ReadonlyTrajectories const& massive_oblate_trajectories,
      ReadonlyTrajectories const& massive_spherical_trajectories,
      ReadonlyTrajectories const& massless_trajectories,
      Instant const& reference_time,
      Time const& t,
      std::vector<Length> const& q,
      std::vector<Acceleration>* result);

  // No transfer of ownership.
  static void ComputeGravitationalVelocities(std::vector<Speed> const& p,
                                             std::vector<Speed>* result);
};

}  // namespace physics
}  // namespace principia

#include "physics/n_body_system_body.hpp"
