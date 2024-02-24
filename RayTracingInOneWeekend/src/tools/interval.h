#pragma once

struct Interval
{
	double min, max;

	// Default is empty interval
	Interval() : min(+infinity), max(-infinity) {}
	Interval(double min, double max) : min(min), max(max) {}

	bool Contains(double x) const 
	{
		return min <= x && x <= max;
	}

	bool Surrounds(double x) const
	{
		return min < x && x < max;
	}

	static const Interval empty, universe;
};

static const Interval empty    (+infinity, -infinity);
static const Interval universe (-infinity, +infinity);