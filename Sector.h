#pragma once
#include "Coord.h"
#include <vector>
#include <cassert>


// Segment each segment has 9 Cols
/*
A,B,C
D,E,F
G,H,I 

Coordinates for sectors are always the coordinate of the top left field!
*/



class Sector {
	static const int sectorWidth = 3;
	enum sectorType { A = 0 , B, C, D, E, F, G, H, I, ERROR };
	Coord myCoord;
	int maxVal = 8;
	// sectorCoords for the top/left corner field of a sector.
	sectorType mySector;

public:

	Coord _coordsForSecNum(sectorType _sector);
	Coord getCoord();

	//// Determines Sector (A - F) of a Coordinate.
	//Sector getCoordSector(Coord pointCoord) {
	//	unsigned int quadrant_x = pointCoord.x / 3;
	//	unsigned int quadrant_y = pointCoord.y / 3;
	//	int sectorNum = quadrant_x + (quadrant_y * 3);

	//	Sector sector(sectorNum);
	//	return sector;
	//}

	std::vector<Coord> coordsInSector();


	// Sets the Sector by a point coordinate
	void setByPointCoord(Coord pointCoord);

	std::string toString();

	Sector(sectorType sector);

	Sector();

	// Delegating Constructor
	Sector(int i) : Sector(static_cast<sectorType>(i)) {
		assert(i <= 8);
	}

};
