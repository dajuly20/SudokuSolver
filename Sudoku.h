#pragma once
#include "Sudoku.h"
#include "Coord.h"
#include "Sector.h"
#include <vector>
#include <cassert>
#include <string>
#include <set>
#include <map>

using namespace std;
enum sanity { SANE, INSANE };

class Sudoku {

	std::vector<std::vector<char>> mySudoku;
	std::vector<std::vector<char>> parseCSV(string);
	int gaps;
	enum sanity sanity = SANE;

public:
	static const int RANGE_MAX = 8;
	static const int VALUE_MAX = 9;
	int verbosity = 0;

	void setVerbosity(int v);

	Sudoku(); // must not be called.
	Sudoku(std::string filename);
	Sudoku(std::vector<std::vector<char>> newSudoku, int _gaps);
	Sudoku(std::vector<std::vector<char>> newSudoku);
	


	string toStr();


	bool sanityAll();
	bool sanityCoord(Coord gap);

	bool sanityRow(int numrow);
	bool sanityCol(int numcol);
	bool sanitySec(Sector sector);

	int getField(Coord field);

	
	std::map<Coord, int> getVectorWithCoords(std::vector<Coord> coords);
	std::vector<char> getVector(std::vector<Coord> coords);

	std::vector<char> getRow(unsigned int i_row);
	std::vector<char> getCol(unsigned int i_col);


	std::map<Coord, int> getSectorWithCoords(Sector sector);
	std::vector<char> getSector(Sector sector);
	std::vector<char> getSector(Coord pointCoord);

	bool numIsInRow(int row, int needle);
	bool numIsInCol(int col, int needle);
	
	bool isSolved();

	int getGaps();
	int getSanity();

	void assign(Coord coord, int val);


};