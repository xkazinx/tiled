#pragma once
// A point / size of any value type (tile, world, render and movement points). The
// old kazilib's Point<T>: arithmetic with another point or a value, As<U>() to
// convert, Abs(), MoveTo (a step of `step` units toward another point, the old
// Project). Trivially copyable: written to a packet as its two values.
#include <cmath>
#include <type_traits>

namespace common
{
template <typename T>
struct Point
{
	using Value = T;

	T x{}, y{};

	constexpr Point() = default;
	template <typename A, typename B>
	constexpr Point(const A x_, const B y_) : x((T)x_), y((T)y_) {}
	template <typename U, typename = std::enable_if_t<!std::is_same_v<U, T>>>
	constexpr Point(const Point<U> & p) : x((T)p.x), y((T)p.y) {}

	constexpr Point operator+(const Point & p) const { return { x + p.x, y + p.y }; }
	constexpr Point operator-(const Point & p) const { return { x - p.x, y - p.y }; }
	constexpr Point operator*(const Point & p) const { return { x * p.x, y * p.y }; }
	constexpr Point operator/(const Point & p) const { return { x / p.x, y / p.y }; }
	constexpr Point operator+(const T v) const { return { x + v, y + v }; }
	constexpr Point operator-(const T v) const { return { x - v, y - v }; }
	constexpr Point operator*(const T v) const { return { x * v, y * v }; }
	constexpr Point operator/(const T v) const { return { x / v, y / v }; }

	constexpr Point & operator+=(const Point & p) { return *this = *this + p; }
	constexpr Point & operator-=(const Point & p) { return *this = *this - p; }
	constexpr Point & operator*=(const Point & p) { return *this = *this * p; }
	constexpr Point & operator/=(const Point & p) { return *this = *this / p; }

	constexpr bool operator==(const Point & p) const { return x == p.x && y == p.y; }
	constexpr bool operator!=(const Point & p) const { return !(*this == p); }

	template <typename U>
	constexpr Point<U> As() const { return { (U)x, (U)y }; }

	Point Abs() const { return { std::abs(x), std::abs(y) }; }

	// A step of `step` toward `to` (the old Project; the caller clamps at the end)
	template <typename P>
	P MoveTo(const P & to, const float step) const
	{
		const auto angle = std::atan2((float)to.y - (float)y, (float)to.x - (float)x);
		return { (typename P::Value)(x + std::cos(angle) * step), (typename P::Value)(y + std::sin(angle) * step) };
	}
};
}
