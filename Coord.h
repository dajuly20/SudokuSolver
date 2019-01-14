#pragma once
#include <string>
#include <sstream>

class Coord {
public:
	unsigned int x;
	unsigned int y;
	const int maxDimen = 8;

//	string _toStr();
	int test();
	std::string toStr();
	Coord();
	Coord(unsigned int _x, unsigned int _y);
	Coord operator =(const Coord& rhs);

	bool operator < (const Coord& rhs) const;
	bool operator > (const Coord& rhs) const;

	Coord getSectorCoord();
	Coord getAbsoluteCoord();

};