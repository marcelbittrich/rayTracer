#pragma once

#include "rtweekend.h"

struct Interval
{
	double min, max;

	// Default is empty interval
	Interval() : min(+infinity), max(-infinity) {}
	Interval(double min, double max) : min(min), max(max) {}
	Interval(const Interval& a, const Interval& b)
	{
		min = a.min <= b.min ? a.min : b.min;
		max = a.max >= b.max ? a.max : b.max;
	}

	double Size() const 
	{
		return max - min;
	}

	bool Contains(double x) const 
	{
		return min <= x && x <= max;
	}

	bool Surrounds(double x) const
	{
		return min < x && x < max;
	}

	Interval Expand(double delta) const 
	{
		double padding = delta / 2;
		return Interval(min - padding, max + padding);
	}

	static const Interval empty, universe;
};

