﻿
#pragma once

#include <string>
#include <vector>

#include "base/not_null.hpp"
#include "gmock/gmock.h"
#include "ksp_plugin/plugin.hpp"

namespace principia {
namespace ksp_plugin {
namespace internal_plugin {

class MockPlugin : public Plugin {
 public:
  MockPlugin();
  MockPlugin(MockPlugin const&) = delete;
  MockPlugin(MockPlugin&&) = delete;
  ~MockPlugin() override = default;

  void InsertCelestialAbsoluteCartesian(
      Index const celestial_index,
      std::experimental::optional<Index> const& parent_index,
      DegreesOfFreedom<Barycentric> const& initial_state,
      base::not_null<std::unique_ptr<MassiveBody const>> body) override;

  MOCK_METHOD4(
      InsertCelestialAbsoluteCartesianConstRef,
      void(Index const celestial_index,
           std::experimental::optional<Index> const& parent_index,
           DegreesOfFreedom<Barycentric> const& initial_state,
           base::not_null<std::unique_ptr<MassiveBody const>> const& body));

  MOCK_METHOD0(EndInitialization,
               void());

  MOCK_CONST_METHOD1(HasEncounteredApocalypse,
                     bool(std::string* const details));

  MOCK_CONST_METHOD2(UpdateCelestialHierarchy,
                     void(Index const celestial_index,
                          Index const parent_index));

  MOCK_METHOD2(InsertOrKeepVessel,
               bool(GUID const& vessel_guid, Index const parent_index));

  MOCK_METHOD2(SetVesselStateOffset,
               void(GUID const& vessel_guid,
                    RelativeDegreesOfFreedom<AliceSun> const& from_parent));

  MOCK_METHOD2(AdvanceTime,
               void(Instant const& t, Angle const& planetarium_rotation));

  MOCK_CONST_METHOD1(ForgetAllHistoriesBefore, void(Instant const& t));

  MOCK_CONST_METHOD1(VesselFromParent,
                     RelativeDegreesOfFreedom<AliceSun>(
                         GUID const& vessel_guid));

  MOCK_CONST_METHOD1(CelestialFromParent,
                     RelativeDegreesOfFreedom<AliceSun>(
                         Index const celestial_index));

  MOCK_CONST_METHOD3(CreateFlightPlan,
                     void(GUID const& vessel_guid,
                          Instant const& final_time,
                          Mass const& initial_mass));

  // NOTE(phl): gMock 1.7.0 doesn't support returning a std::unique_ptr<>.  So
  // we override the function of the Plugin class with bona fide functions which
  // call mock functions which fill a std::unique_ptr<> instead of returning it.
  not_null<std::unique_ptr<DiscreteTrajectory<World>>> RenderedVesselTrajectory(
      GUID const& vessel_guid,
      Position<World> const& sun_world_position) const override;
  MOCK_CONST_METHOD3(FillRenderedVesselTrajectory,
                     void(GUID const& vessel_guid,
                          Position<World> const& sun_world_position,
                          std::unique_ptr<DiscreteTrajectory<World>>*
                              rendered_vessel_trajectory));

  not_null<std::unique_ptr<DiscreteTrajectory<World>>> RenderedPrediction(
      GUID const& vessel_guid,
      Position<World> const& sun_world_position) const override;
  MOCK_CONST_METHOD3(
      FillRenderedPrediction,
      void(GUID const& vessel_guid,
           Position<World> const& sun_world_position,
           std::unique_ptr<DiscreteTrajectory<World>>* rendered_prediction));

  not_null<std::unique_ptr<DiscreteTrajectory<World>>>
  RenderedTrajectoryFromIterators(
      DiscreteTrajectory<Barycentric>::Iterator const& begin,
      DiscreteTrajectory<Barycentric>::Iterator const& end,
      Position<World> const& sun_world_position) const;
  MOCK_CONST_METHOD4(
      FillRenderedTrajectoryFromIterators,
      void(DiscreteTrajectory<Barycentric>::Iterator const& begin,
           DiscreteTrajectory<Barycentric>::Iterator const& end,
           Position<World> const& sun_world_position,
           std::unique_ptr<DiscreteTrajectory<World>>*
               rendered_trajectory_from_iterators));

  MOCK_METHOD1(SetPredictionLength, void(Time const& t));

  MOCK_METHOD1(SetPredictionAdaptiveStepParameters,
               void(Ephemeris<Barycentric>::AdaptiveStepParameters const&
                        prediction_adaptive_step_parameters));

  MOCK_CONST_METHOD1(HasVessel, bool(GUID const& vessel_guid));
  MOCK_CONST_METHOD1(GetVessel, not_null<Vessel*>(GUID const& vessel_guid));

  not_null<std::unique_ptr<NavigationFrame>>
  NewBodyCentredNonRotatingNavigationFrame(
      Index const reference_body_index) const override;
  not_null<std::unique_ptr<NavigationFrame>>
  NewBarycentricRotatingNavigationFrame(
      Index const primary_index,
      Index const secondary_index) const override;

  MOCK_CONST_METHOD2(
      FillBodyCentredNonRotatingNavigationFrame,
      void(Index const reference_body_index,
           std::unique_ptr<NavigationFrame>* navigation_frame));
  MOCK_CONST_METHOD3(
      FillBarycentricRotatingNavigationFrame,
      void(Index const primary_index,
           Index const secondary_index,
           std::unique_ptr<NavigationFrame>* navigation_frame));

  // NOTE(phl): Needed because gMock 1.7.0 wants to copy the unique_ptr<>.
  void SetPlottingFrame(
    not_null<std::unique_ptr<NavigationFrame>> plotting_frame) override;

  MOCK_METHOD1(SetPlottingFrameConstRef,
               void(NavigationFrame const& plotting_frame));

  MOCK_CONST_METHOD0(GetPlottingFrame,
                     not_null<NavigationFrame const*>());

  // NOTE(phl): Another wrapper needed because gMock 1.7.0 wants to copy the
  // vector of unique_ptr<>.
  void AddVesselToNextPhysicsBubble(GUID const& vessel_guid,
                                    std::vector<IdAndOwnedPart> parts) override;

  MOCK_METHOD2(AddVesselToNextPhysicsBubbleConstRef,
               void(GUID const& vessel_guid,
                    std::vector<IdAndOwnedPart> const& parts));

  MOCK_CONST_METHOD0(PhysicsBubbleIsEmpty, bool());

  MOCK_CONST_METHOD1(BubbleDisplacementCorrection,
                     Displacement<World>(
                         Position<World> const& sun_world_position));

  MOCK_CONST_METHOD1(BubbleVelocityCorrection,
                     Velocity<World>(
                         Index const reference_body_index));

  MOCK_CONST_METHOD1(NavballFrameField,
                     std::unique_ptr<FrameField<World, Navball>>(
                         Position<World> const& sun_world_position));

  MOCK_CONST_METHOD1(VesselTangent,
                     Vector<double, World>(GUID const& vessel_guid));

  MOCK_CONST_METHOD1(VesselNormal,
                     Vector<double, World>(GUID const& vessel_guid));

  MOCK_CONST_METHOD1(VesselBinormal,
                     Vector<double, World>(GUID const& vessel_guid));

  MOCK_CONST_METHOD1(VesselVelocity,
                     Velocity<World>(GUID const& vessel_guid));

  MOCK_CONST_METHOD0(BarycentricToWorldSun,
                     OrthogonalMap<Barycentric, WorldSun>());

  MOCK_CONST_METHOD0(CurrentTime, Instant());

  MOCK_CONST_METHOD1(WriteToMessage,
                     void(not_null<serialization::Plugin*> const message));
};

}  // namespace internal_plugin

using internal_plugin::MockPlugin;

}  // namespace ksp_plugin
}  // namespace principia
