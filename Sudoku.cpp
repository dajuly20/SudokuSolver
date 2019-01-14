//#include "stdafx.h"
#include "Sudoku.h"
#include <iostream>
#include <fstream>
#include <set>
#include <map>






void Sudoku::setVerbosity(int v) {
	this->verbosity = v;
}

Sudoku::Sudoku() {
	this->gaps = (RANGE_MAX + 1) ^ 2;
	this->sanity = INSANE;
}


Sudoku::Sudoku(std::string filename) {
	this->mySudoku = this->parseCSV(filename);
}


Sudoku::Sudoku(std::vector<std::vector<char>> newSudoku, int _gaps) {
	this->mySudoku = newSudoku;
	this->gaps = _gaps;
}

Sudoku::Sudoku(std::vector<std::vector<char>> newSudoku) {
	this->mySudoku = newSudoku;
	this->gaps = 0;
	for (int ix = 0; ix <= RANGE_MAX; ix++) {
		for (int iy = 0; iy <= RANGE_MAX; iy++) {
			if (mySudoku[iy][ix] == 0) this->gaps++;
		}
	}
}





std::vector<std::vector<char> > Sudoku::parseCSV(string filename) {
	std::ifstream data(filename);
	std::string line;
	std::vector<std::vector<char> > parsedCsv;

	this->gaps = 0;

	while (std::getline(data, line))
	{
		
		std::stringstream lineStream(line);
		std::string cell;
		std::vector<char> parsedRow;
		while (std::getline(lineStream, cell, ','))
		{
			int value = std::stoi(cell);
			if (value == 0) this->gaps++;
			parsedRow.push_back(value);
		}

		parsedCsv.push_back(parsedRow);
	}
	return parsedCsv;
}




string Sudoku::toStr() {
		std::stringstream out;
		
		

	for (int i = 0; i < mySudoku.size(); i++)
	{
		for (int j = 0; j < mySudoku[i].size(); j++)
		{
			int value = (int)mySudoku[i][j];
			out << value << " | ";
		}
		out << "\n";
	}

	return out.str();


	//if (hits == gaps) {
	//	cout << "Sudoku was solved!! " << endl;
	//}
	//else {
	//	cout << "Sudoku not solved :-( " << endl;
	//}
	//cout << "Sanity: " << ((this->sanity == 0) ? "SANE" : "INSANE") << endl;
	//cout << "Hits: " << this->hits << endl;
	//cout << "Iterations" << this->iterations << endl;
	//cout << "Gaps: " << this->gaps << endl;
	//cout << "Two chances: " << this->twoChances << endl;

}






bool Sudoku::sanityAll() {
	static const int maxDimen = 8;
	bool res = true;
	for (int i = 0; i <= maxDimen; i++) {

		if (!sanityRow(i) || !sanityCol(i) || !sanitySec(Sector(i))) {
			this->sanity = INSANE;
			return false;
		} 

		

	}
	this->sanity = SANE;
	return true;
}

bool Sudoku::sanityCoord(Coord gap) {

	Sector sector = Sector();
	sector.setByPointCoord(gap);

	// First one to return zero and zero will be returned.
	bool ret = 	(sanityRow(gap.y) &&
				 sanityCol(gap.x) &&
				 sanitySec(sector));
	this->sanity = (ret ? SANE : INSANE);
	return this->sanity;
}

bool Sudoku::sanityRow(int numrow) {
	std::vector<char> row = getRow(numrow);

	set<int> uniqueVals;

	for (auto const& val : row) {
		if (val != 0) {
			//pair<set<int>::iterator, bool> result = uniqueVals.insert(val);
			if (uniqueVals.insert(val).second == false) {
				if (this->verbosity >= 1) cout << " Sanity failed (row " << numrow << ") val:" << val << endl;
				this->sanity = INSANE;
				return false;
			}
		}
	
	}
	this->sanity = SANE;
	return true;
}

bool Sudoku::sanityCol(int numcol) {
	std::vector<char> col = getCol(numcol);
	set<int> uniqueVals;

	for (auto const& val : col) {
		if (val != 0) {
			//pair<set<int>::iterator, bool> result = uniqueVals.insert(val);
			if (uniqueVals.insert(val).second == false) {
				if (this->verbosity >= 1) cout << " Sanity failed (col " << numcol << ") val:" << val << endl;
				this->sanity = INSANE;
				return false;
			}
		}
	}
	this->sanity = SANE;
	return true;
}

bool Sudoku::sanitySec(Sector sector) {
	std::vector<char> sec = getSector(sector);
	set<int> uniqueVals;

	for (auto const& val : sec) {
		if (val != 0) {
			//pair<set<int>::iterator, bool> result = uniqueVals.insert(val);
			if (uniqueVals.insert(val).second == false) {
				if (this->verbosity >= 1) cout << " Sanity failed (sector " << sector.toString() << ") val:" << val << endl;
				this->sanity = INSANE;
				return false;
			}
		}
	}
	this->sanity = SANE;
	return true;
}



int Sudoku::getField(Coord field) {
	return mySudoku[field.y][field.x];
}



std::map<Coord, int> Sudoku::getVectorWithCoords(std::vector<Coord> coords) {
	std::map<Coord, int> resultVec;
	for (auto const& coord : coords) {
		int val = this->mySudoku[coord.y][coord.x];
		resultVec.insert(make_pair(coord,val));
	}

	return resultVec;
}


std::vector<char> Sudoku::getVector(std::vector<Coord> coords) {
	std::vector<char> resultVec;
	for (auto const& coord : coords) {
		int val = this->mySudoku[coord.y][coord.x];
		resultVec.push_back(val);
	}

	return resultVec;
}


std::vector<char> Sudoku::getRow(unsigned int i_row) {
	return this->mySudoku[i_row];
}

std::vector<char> Sudoku::getCol(unsigned int i_col) {
	std::vector<char> resultVec;
	for (int i = 0; i <= RANGE_MAX; i++) {
		int val = this->mySudoku[i][i_col];
		resultVec.push_back(val);
	}

	return resultVec;
}



std::map<Coord, int> Sudoku::getSectorWithCoords(Sector sector) {
	std::vector<Coord> coords = sector.coordsInSector();

	std::map<Coord, int> result;
	result =  getVectorWithCoords (coords);
		
	return result;
}


std::vector<char> Sudoku::getSector(Sector sector) {
	std::vector<Coord> coords = sector.coordsInSector();
	std::vector<char> resultVec;
	resultVec = getVector(coords);
	return resultVec;
}

std::vector<char> Sudoku::getSector(Coord pointCoord) {
	Sector sector = Sector();
	sector.setByPointCoord(pointCoord);
	std::vector<Coord> coords = sector.coordsInSector();
	std::vector<char> resultVec;
	resultVec = getVector(coords);
	return resultVec;
}

bool Sudoku::numIsInRow(int row, int needle) {
	std::vector<char> rowVec = this->getRow(row);
	std::set<int> haystack(rowVec.begin(), rowVec.end());
	if (haystack.find(needle) != haystack.end()) {
		return true;
	}
	else {
		return false;
	}
}

bool Sudoku::numIsInCol(int col, int needle) {
	std::vector<char> colVec = this->getCol(col);
	std::set<int> haystack(colVec.begin(), colVec.end());
	if (haystack.find(needle) != haystack.end()) {
		return true;
	}
	else {
		return false;
	}
}


bool Sudoku::isSolved() {
	return (this->gaps == 0 && this->sanity == SANE);
}

int Sudoku::getGaps() {
	return this->gaps;
}

int Sudoku::getSanity() {
	return this->sanity;
}

void Sudoku::assign(Coord coord, int val) {
	assert(val > 0 && val <= 9);
	if (mySudoku[coord.y][coord.x] == 0) {
		this->gaps--;
		this->mySudoku[coord.y][coord.x] = val;
	}
	else {
		if (this->verbosity >= 1) {
			cout << endl << endl << endl;
			cout << "-------------------------------------" << endl;
			cout << " Waring: Tried to re-assign Coord " << coord.toStr() << " with value " << val << endl;
			cout << "-------------------------------------" << endl;
			cout << endl << endl << endl;
		}
	}
}


