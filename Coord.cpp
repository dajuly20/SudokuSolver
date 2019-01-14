//#include "stdafx.h"
#include <cassert>
#include <string>
#include <sstream>
#include "Sudoku.h"
#include "Coord.h"


using namespace std;


std::string Coord::toStr() {
	std::stringstream out;
	out << "(" << x << "x /" << y << "y)";
	return out.str();
}



	bool Coord::operator < (const Coord& rhs) const{
		int rel_lhs = (y * Sudoku::VALUE_MAX) + x;
		int rel_rhs = (rhs.y * Sudoku::VALUE_MAX) + rhs.x;

		return rel_lhs < rel_rhs;
	}

	bool Coord::operator > (const Coord& rhs) const{
		int rel_lhs = (y * Sudoku::VALUE_MAX) + x;
		int rel_rhs = (rhs.y *Sudoku::VALUE_MAX) + rhs.x;

		return rel_lhs > rel_rhs;
	}

	Coord::Coord() {
		x = 0;
		y = 0;
	}

	Coord::Coord(unsigned int _x, unsigned int _y) {
		assert(_x <= maxDimen);
		assert(_y <= maxDimen);
		x = _x;
		y = _y;
	}
	



	Coord Coord::operator =(const Coord& rhs) {
		this->x = rhs.x;
		this->y = rhs.y;
		return *this;
	}



	Coord Coord::getSectorCoord(){
		return Coord(
					this->x / 3,
					this->y / 3);
	}

	Coord Coord::getAbsoluteCoord() {
		return Coord(
			this->x * 3,
			this->y * 3
		);
	}
