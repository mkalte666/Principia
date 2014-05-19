#pragma once

namespace principia {
namespace geometry {

template<typename Vector>
class Point {
 public:
  explicit Point(Vector const& coordinates);

  template<typename Vector>
  friend Vector operator-(Point<Vector> const& to, Point<Vector> const& from);

  template<typename Vector>
  friend Point<Vector> operator+(Point<Vector> const& left,
                                 Vector const& right);
  template<typename Vector>
  friend Point<Vector> operator+(Vector const& left,
                                 Point<Vector> const& right);
  template<typename Vector>
  friend Point<Vector> operator-(Point<Vector> const& left,
                                 Vector const& right);

  template<typename Vector>
  friend bool operator==(Point<Vector> const& left, Point<Vector> const& right);
  template<typename Vector>
  friend bool operator!=(Point<Vector> const& left, Point<Vector> const& right);

  template<typename Vector, typename Weight>
  friend Point<Vector> Barycentre(Point<Vector> const& left,
                                  Weight const& left_weight,
                                  Point<Vector> const& right,
                                  Weight const& right_weight);

 private:
  Vector coordinates_;
};

template<typename Vector>
Vector operator-(Point<Vector> const& to, Point<Vector> const& from);

template<typename Vector>
Point<Vector> operator+(Point<Vector> const& left, Vector const& right);
template<typename Vector>
Point<Vector> operator+(Vector const& left, Point<Vector> const& right);
template<typename Vector>
Point<Vector> operator-(Point<Vector> const& left, Vector const& right);

template<typename Vector>
bool operator==(Point<Vector> const& left, Point<Vector> const& right);
template<typename Vector>
bool operator!=(Point<Vector> const& left, Point<Vector> const& right);

template<typename Vector, typename Weight>
Point<Vector> Barycentre(Point<Vector> const& left,
                         Weight const& left_weight,
                         Point<Vector> const& right,
                         Weight const& right_weight);

}  // namespace geometry
}  // namespace principia

#include "geometry/affine_space_body.hpp"
