//#include "stdafx.h"
#include "Sector.h"

// Returns Coords for topLeft field of sector
	Coord Sector::_coordsForSecNum(sectorType _sector) {
		int sector = _sector;
		int y = (sector / sectorWidth) * sectorWidth; // INTEGER division.. so e.g. 8/3 => 2 => 2 *3 = 6
		int x = (sector % sectorWidth) * sectorWidth;
		return Coord(x, y);
	}




	//// Determines Sector (A - F) of a Coordinate.
	//Sector getCoordSector(Coord pointCoord) {
	//	unsigned int quadrant_x = pointCoord.x / 3;
	//	unsigned int quadrant_y = pointCoord.y / 3;
	//	int sectorNum = quadrant_x + (quadrant_y * 3);

	//	Sector sector(sectorNum);
	//	return sector;
	//}

	std::vector<Coord> Sector::coordsInSector() {
		std::vector<Coord> cordsInSector;
		int startX = myCoord.x;
		int startY = myCoord.y;
		//cout << "Coords in Sector: " << myCoord.toStr() << " #" << mySector << endl;
		for (int ix = 0; ix < sectorWidth; ix++) {
			for (int iy = 0; iy < sectorWidth; iy++) {
				cordsInSector.push_back(Coord(startX + ix, startY + iy));
			}
		}

		return cordsInSector;
	}


	// Sets the Sector by a point coordinate
	void Sector::setByPointCoord(Coord pointCoord) {
		// Integer division. So 7 /3 becomes 2 
		unsigned int quadrant_x = (pointCoord.x / 3) * 3;
		unsigned int quadrant_y = (pointCoord.y / 3) * 3;

		//cout << "Setzen mit Koordinate:" << pointCoord.toStr() << endl;

		myCoord = Coord(quadrant_x, quadrant_y);

		//cout << "Sektor hat Koordinate:" << myCoord.toStr() << endl;


		int sectorNum = quadrant_x/3 + (quadrant_y);
		mySector = static_cast<sectorType>(sectorNum);

	}

	std::string Sector::toString() {
		switch (mySector) {
		case A: return "A"; break;
		case B: return "B"; break;
		case C: return "C"; break;
		case D: return "D"; break;
		case E: return "E"; break;
		case F: return "F"; break;
		case G: return "G"; break;
		case H: return "H"; break;
		case I: return "I"; break;
		}
	}

	Sector::Sector(sectorType sector) {
		mySector = sector;
		myCoord = _coordsForSecNum(sector);
	}

	Coord Sector::getCoord() {
		return myCoord;
	}

	Sector::Sector() {
		mySector = ERROR;
		myCoord = Coord();
	}

	