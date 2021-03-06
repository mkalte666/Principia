﻿
#include "physics/body.hpp"

#include "geometry/named_quantities.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "physics/massive_body.hpp"
#include "physics/massless_body.hpp"
#include "physics/oblate_body.hpp"
#include "physics/rotating_body.hpp"
#include "quantities/si.hpp"
#include "serialization/geometry.pb.h"
#include "testing_utilities/almost_equals.hpp"

namespace principia {
namespace physics {
namespace internal_body {

using geometry::AngularVelocity;
using geometry::Frame;
using geometry::Instant;
using geometry::Normalize;
using geometry::RadiusLatitudeLongitude;
using geometry::Vector;
using quantities::AngularFrequency;
using quantities::GravitationalParameter;
using quantities::Order2ZonalCoefficient;
using quantities::si::Degree;
using quantities::si::Metre;
using quantities::si::Radian;
using quantities::si::Second;
using testing_utilities::AlmostEquals;
using ::testing::IsNull;
using ::testing::NotNull;

class BodyTest : public testing::Test {
 protected:
  using World = Frame<serialization::Frame::TestTag,
                      serialization::Frame::TEST, true>;

  // We need that so the comma doesn't get caught in macros.
  using Direction = Vector<double, World>;

  template<typename Tag, Tag tag>
  void TestRotatingBody() {
    using F = Frame<Tag, tag, true>;

    auto const rotating_body =
        RotatingBody<F>(17 * SIUnit<GravitationalParameter>(),
                        typename RotatingBody<F>::Parameters(
                            2 * Metre,
                            3 * Radian,
                            Instant() + 4 * Second,
                            angular_frequency_,
                            right_ascension_of_pole_,
                            declination_of_pole_));

    serialization::Body message;
    RotatingBody<F> const* cast_rotating_body;
    rotating_body.WriteToMessage(&message);
    EXPECT_TRUE(message.has_massive_body());
    EXPECT_FALSE(message.has_massless_body());
    EXPECT_TRUE(message.massive_body().HasExtension(
                    serialization::RotatingBody::extension));

    not_null<std::unique_ptr<MassiveBody const>> const massive_body =
        MassiveBody::ReadFromMessage(message);
    EXPECT_EQ(rotating_body.gravitational_parameter(),
              massive_body->gravitational_parameter());
    cast_rotating_body =
        dynamic_cast_not_null<RotatingBody<F> const*>(massive_body.get());
    EXPECT_THAT(cast_rotating_body, NotNull());
  }

  AngularFrequency const angular_frequency_ = -1.5 * Radian / Second;
  Angle const right_ascension_of_pole_ = 37 * Degree;
  Angle const declination_of_pole_ = 123 * Degree;

  MasslessBody massless_body_;
  MassiveBody massive_body_ =
      MassiveBody(42 * SIUnit<GravitationalParameter>());
  RotatingBody<World> rotating_body_ =
      RotatingBody<World>(17 * SIUnit<GravitationalParameter>(),
                          RotatingBody<World>::Parameters(
                              1 * Metre,
                              3 * Radian,
                              Instant() + 4 * Second,
                              angular_frequency_,
                              right_ascension_of_pole_,
                              declination_of_pole_));
  OblateBody<World> oblate_body_ =
      OblateBody<World>(17 * SIUnit<GravitationalParameter>(),
                        RotatingBody<World>::Parameters(
                            1 * Metre,
                            3 * Radian,
                            Instant() + 4 * Second,
                            angular_frequency_,
                            right_ascension_of_pole_,
                            declination_of_pole_),
                        OblateBody<World>::Parameters(
                            163 * SIUnit<Order2ZonalCoefficient>()));
};

using BodyDeathTest = BodyTest;

TEST_F(BodyTest, MasslessSerializationSuccess) {
  EXPECT_TRUE(massless_body_.is_massless());
  EXPECT_FALSE(massless_body_.is_oblate());

  serialization::Body message;
  MasslessBody const* cast_massless_body;
  massless_body_.WriteToMessage(&message);
  EXPECT_TRUE(message.has_massless_body());
  EXPECT_FALSE(message.has_massive_body());

  // Direct deserialization.
  // No members to test in this class, we just check that it doesn't crash.
  massless_body_ = *MasslessBody::ReadFromMessage(message);

  // Dispatching from |Body|.  Need two steps to add const and remove
  // |not_null|.
  not_null<std::unique_ptr<Body const>> body = Body::ReadFromMessage(message);
  cast_massless_body = dynamic_cast_not_null<MasslessBody const*>(body.get());
  EXPECT_THAT(cast_massless_body, NotNull());
}

// The best serialization revenge.
TEST_F(BodyTest, MassiveSerializationSuccess) {
  EXPECT_FALSE(massive_body_.is_massless());
  EXPECT_FALSE(massive_body_.is_oblate());

  serialization::Body message;
  MassiveBody const* cast_massive_body;
  massive_body_.WriteToMessage(&message);
  EXPECT_TRUE(message.has_massive_body());
  EXPECT_FALSE(message.has_massless_body());
  EXPECT_EQ(42, message.massive_body().gravitational_parameter().magnitude());

  // Direct deserialization.
  MassiveBody const massive_body = *MassiveBody::ReadFromMessage(message);
  EXPECT_EQ(massive_body_.gravitational_parameter(),
            massive_body.gravitational_parameter());

  // Dispatching from |Body|.
  not_null<std::unique_ptr<Body>> body = Body::ReadFromMessage(message);
  cast_massive_body = dynamic_cast_not_null<MassiveBody*>(body.get());
  EXPECT_THAT(cast_massive_body, NotNull());
  EXPECT_EQ(massive_body_.gravitational_parameter(),
            cast_massive_body->gravitational_parameter());
}

TEST_F(BodyTest, RotatingSerializationSuccess) {
  EXPECT_FALSE(rotating_body_.is_massless());
  EXPECT_FALSE(rotating_body_.is_oblate());

  serialization::Body message;
  RotatingBody<World> const* cast_rotating_body;
  rotating_body_.WriteToMessage(&message);
  EXPECT_TRUE(message.has_massive_body());
  EXPECT_FALSE(message.has_massless_body());
  EXPECT_TRUE(message.massive_body().HasExtension(
                  serialization::RotatingBody::extension));
  EXPECT_EQ(17, message.massive_body().gravitational_parameter().magnitude());
  serialization::RotatingBody const rotating_body_extension =
      message.massive_body().GetExtension(
          serialization::RotatingBody::extension);
  EXPECT_EQ(3, rotating_body_extension.reference_angle().magnitude());
  EXPECT_EQ(4,
            rotating_body_extension.reference_instant().scalar().magnitude());
  EXPECT_EQ(angular_frequency_,
            AngularFrequency::ReadFromMessage(
                rotating_body_extension.angular_frequency()));
  EXPECT_EQ(right_ascension_of_pole_,
            Angle::ReadFromMessage(
                rotating_body_extension.right_ascension_of_pole()));
  EXPECT_EQ(declination_of_pole_,
            Angle::ReadFromMessage(
                rotating_body_extension.declination_of_pole()));

  // Dispatching from |MassiveBody|.
  not_null<std::unique_ptr<MassiveBody const>> const massive_body =
      MassiveBody::ReadFromMessage(message);
  EXPECT_EQ(rotating_body_.gravitational_parameter(),
            massive_body->gravitational_parameter());
  cast_rotating_body =
      dynamic_cast_not_null<RotatingBody<World> const*>(massive_body.get());
  EXPECT_THAT(cast_rotating_body, NotNull());
  EXPECT_EQ(rotating_body_.gravitational_parameter(),
            cast_rotating_body->gravitational_parameter());
  EXPECT_EQ(rotating_body_.angular_velocity(),
            cast_rotating_body->angular_velocity());
  EXPECT_EQ(rotating_body_.AngleAt(Instant()),
            cast_rotating_body->AngleAt(Instant()));

  // Dispatching from |Body|.
  not_null<std::unique_ptr<Body const>> const body =
      Body::ReadFromMessage(message);
  cast_rotating_body =
      dynamic_cast_not_null<RotatingBody<World> const*>(body.get());
  EXPECT_THAT(cast_rotating_body, NotNull());
  EXPECT_EQ(rotating_body_.gravitational_parameter(),
            cast_rotating_body->gravitational_parameter());
  EXPECT_EQ(rotating_body_.angular_velocity(),
            cast_rotating_body->angular_velocity());
  EXPECT_EQ(rotating_body_.AngleAt(Instant()),
            cast_rotating_body->AngleAt(Instant()));
}

TEST_F(BodyTest, OblateSerializationSuccess) {
  EXPECT_FALSE(oblate_body_.is_massless());
  EXPECT_TRUE(oblate_body_.is_oblate());

  serialization::Body message;
  OblateBody<World> const* cast_oblate_body;
  oblate_body_.WriteToMessage(&message);
  EXPECT_TRUE(message.has_massive_body());
  EXPECT_FALSE(message.has_massless_body());
  EXPECT_TRUE(message.massive_body().GetExtension(
                  serialization::RotatingBody::extension).
                      HasExtension(serialization::OblateBody::extension));
  EXPECT_EQ(17, message.massive_body().gravitational_parameter().magnitude());
  serialization::OblateBody const oblate_body_extension =
      message.massive_body().GetExtension(
                  serialization::RotatingBody::extension).
                      GetExtension(serialization::OblateBody::extension);
  EXPECT_EQ(163, oblate_body_extension.j2().magnitude());

  // Dispatching from |MassiveBody|.
  not_null<std::unique_ptr<MassiveBody const>> const massive_body =
      MassiveBody::ReadFromMessage(message);
  EXPECT_EQ(oblate_body_.gravitational_parameter(),
            massive_body->gravitational_parameter());
  cast_oblate_body =
      dynamic_cast_not_null<OblateBody<World> const*>(massive_body.get());
  EXPECT_THAT(cast_oblate_body, NotNull());
  EXPECT_EQ(oblate_body_.gravitational_parameter(),
            cast_oblate_body->gravitational_parameter());
  EXPECT_EQ(oblate_body_.j2(), cast_oblate_body->j2());
  EXPECT_EQ(oblate_body_.polar_axis(), cast_oblate_body->polar_axis());

  // Dispatching from |Body|.
  not_null<std::unique_ptr<Body const>> const body =
      Body::ReadFromMessage(message);
  cast_oblate_body =
      dynamic_cast_not_null<OblateBody<World> const*>(body.get());
  EXPECT_THAT(cast_oblate_body, NotNull());
  EXPECT_EQ(oblate_body_.gravitational_parameter(),
            cast_oblate_body->gravitational_parameter());
  EXPECT_EQ(oblate_body_.j2(), cast_oblate_body->j2());
  EXPECT_EQ(oblate_body_.polar_axis(), cast_oblate_body->polar_axis());
}

TEST_F(BodyTest, PreBrouwerOblateDeserializationSuccess) {
  EXPECT_FALSE(oblate_body_.is_massless());
  EXPECT_TRUE(oblate_body_.is_oblate());

  // Serialize post-Brouwer.
  serialization::Body post_brouwer_body;
  oblate_body_.WriteToMessage(&post_brouwer_body);
  serialization::MassiveBody const& post_brouwer_massive_body =
     post_brouwer_body.massive_body();
  serialization::RotatingBody const& post_brouwer_rotating_body =
     post_brouwer_massive_body.GetExtension(
        serialization::RotatingBody::extension);
  serialization::OblateBody const& post_brouwer_oblate_body =
     post_brouwer_rotating_body.GetExtension(
        serialization::OblateBody::extension);

  // Construct explicitly an equivalent pre-Brouwer message.
  serialization::Body pre_brouwer_body;
  serialization::MassiveBody* pre_brouwer_massive_body =
      pre_brouwer_body.mutable_massive_body();
  pre_brouwer_massive_body->mutable_gravitational_parameter()->CopyFrom(
      post_brouwer_massive_body.gravitational_parameter());
  serialization::PreBrouwerOblateBody* pre_brouwer_oblate_body =
      pre_brouwer_massive_body->MutableExtension(
          serialization::PreBrouwerOblateBody::extension);
  pre_brouwer_oblate_body->mutable_frame()->CopyFrom(
      post_brouwer_rotating_body.frame());
  pre_brouwer_oblate_body->mutable_j2()->CopyFrom(
      post_brouwer_oblate_body.j2());
  pre_brouwer_oblate_body->mutable_axis()->mutable_frame()->CopyFrom(
      post_brouwer_rotating_body.frame());
  auto const axis_coordinates = RadiusLatitudeLongitude(
                                    1.0,
                                    declination_of_pole_,
                                    right_ascension_of_pole_).ToCartesian();
  auto* const pre_brouwer_axis_r3_element =
    pre_brouwer_oblate_body->mutable_axis()->mutable_vector();
  pre_brouwer_axis_r3_element->mutable_x()->set_double_(axis_coordinates.x);
  pre_brouwer_axis_r3_element->mutable_y()->set_double_(axis_coordinates.y);
  pre_brouwer_axis_r3_element->mutable_z()->set_double_(axis_coordinates.z);

  // Deserialize both.
  auto const post_brouwer = Body::ReadFromMessage(post_brouwer_body);
  auto const pre_brouwer = Body::ReadFromMessage(pre_brouwer_body);
  auto const* const cast_post_brouwer =
      dynamic_cast_not_null<OblateBody<World> const*>(post_brouwer.get());
  auto const* const cast_pre_brouwer =
      dynamic_cast_not_null<OblateBody<World> const*>(pre_brouwer.get());
  EXPECT_EQ(cast_post_brouwer->mass(), cast_pre_brouwer->mass());
  EXPECT_THAT(cast_post_brouwer->polar_axis(),
              AlmostEquals(cast_pre_brouwer->polar_axis(), 2));
  EXPECT_EQ(cast_post_brouwer->j2(), cast_pre_brouwer->j2());
}

TEST_F(BodyTest, AllFrames) {
  TestRotatingBody<serialization::Frame::PluginTag,
                   serialization::Frame::ALICE_SUN>();
  TestRotatingBody<serialization::Frame::PluginTag,
                   serialization::Frame::ALICE_WORLD>();
  TestRotatingBody<serialization::Frame::PluginTag,
                  serialization::Frame::BARYCENTRIC>();
  TestRotatingBody<serialization::Frame::PluginTag,
                   serialization::Frame::PRE_BOREL_BARYCENTRIC>();
  TestRotatingBody<serialization::Frame::PluginTag,
                   serialization::Frame::NAVIGATION>();
  TestRotatingBody<serialization::Frame::PluginTag,
                  serialization::Frame::WORLD>();
  TestRotatingBody<serialization::Frame::PluginTag,
                  serialization::Frame::WORLD_SUN>();

  TestRotatingBody<serialization::Frame::SolarSystemTag,
                  serialization::Frame::ICRF_J2000_ECLIPTIC>();
  TestRotatingBody<serialization::Frame::SolarSystemTag,
                  serialization::Frame::ICRF_J2000_EQUATOR>();

  TestRotatingBody<serialization::Frame::TestTag,
                  serialization::Frame::TEST>();
  TestRotatingBody<serialization::Frame::TestTag,
                  serialization::Frame::TEST1>();
  TestRotatingBody<serialization::Frame::TestTag,
                   serialization::Frame::TEST2>();
  TestRotatingBody<serialization::Frame::TestTag,
                   serialization::Frame::FROM>();
  TestRotatingBody<serialization::Frame::TestTag,
                   serialization::Frame::THROUGH>();
  TestRotatingBody<serialization::Frame::TestTag,
                   serialization::Frame::TO>();
}

}  // namespace internal_body
}  // namespace physics
}  // namespace principia
