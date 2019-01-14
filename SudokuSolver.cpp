// SudokuSolver.cpp: Definiert den Einstiegspunkt für die Konsolenanwendung.
//

//#include "stdafx.h"
#include "Coord.h"
#include "Sector.h"
#include "Sudoku.h"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <set>
#include <map>
#include <set>
#include <bitset>
#include <chrono>

static int recursiveIterations = 0;


using namespace std;

template <int>
class RecursionCounter
{
public:
	inline RecursionCounter() { ++cnt; }
	inline ~RecursionCounter() { --cnt; }
	inline operator int() const { return cnt; }
private:
	static int cnt;

};
template <int num>
int RecursionCounter<num>::cnt = 0;


class SudokuSolver {
	// First X from 0 to 8, then Y from 0 to 8
	/*Sector A - F 
	     ___________________________
	* 0  |        |        |        |
	* 1  |   A    |    B   |    C   | 
	* 2  |________|________|________|
	* 3  |        |        |        |
	* 4  |   D    |    E   |    F   |
	* 5  |________|________|________|
	* 6  |        |        |        |
	* 7  |   G    |    H   |    I   |
	*y8  |________|________|________|
	*x    0, 1, 2,  3, 4, 5,  6, 7, 8
	*/
	const int maxCoord = 8;
	//std::vector<std::vector<int>> mySudoku;
	Sudoku mySudoku;
	std::map<Coord, std::set<int>> candidates;
	std::map<Coord, std::set<int>> candidatesDoubles; 
	std::map<Coord, int> hitsMap;
	std::set<int> fullrange = { 1,2,3,4,5,6,7,8,9 };
	int CANDIDATES_MAX = 9;
	int hits;
	int iterations;
	int twoChances;
	int verbosity = 0;
public:

	void setVerbosity(int v) {
		verbosity = v;
	}

	/************************************
	***               Constructors
	***
	***/
	SudokuSolver(std::string filename) {
		this->mySudoku = Sudoku(filename);
	}


	SudokuSolver(Sudoku newSudoku) {
		this->mySudoku = newSudoku;

	}




	/****************************************
	***
	***           Print Funktions
	***
	***/

	void printHits() {
		cout << "Found " << hitsMap.size() << " hits:" << endl;
		for (auto const& hit : hitsMap) {
			Coord hitCoord = hit.first;
			int   hitValue = hit.second;
			cout << "Hit at " << hitCoord.toStr() << " = " << hitValue << endl;
		}
	}

	void printCandidates() {
		cout << "up to 4 possible Candidates for: " << endl;
		for (auto const& aDouble : candidates) {
			Coord gapCoord = aDouble.first;
			std::set<int> candidates = aDouble.second;
			cout << "Coord " << gapCoord.toStr() << " ";
			for (int candidate : candidates) {
				cout << candidate << ", ";
			}
			cout << endl;
		}
	}

	void printCandidateDoubles() {
		cout << "2 possible Candidates for: " << endl;
		for (auto const& aDouble : candidatesDoubles) {
			Coord gapCoord = aDouble.first;
			std::set<int> candidates = aDouble.second;
			cout << "Coord " << gapCoord.toStr() << " ";
			for (int candidate : candidates) {
				cout << candidate << ", ";
			}
			cout << endl;
		}
	}




	void print() {
		cout << mySudoku.toStr();
		if (mySudoku.isSolved()) {
			cout << "Sudoku was solved!! " << endl;
		}
		else {
			cout << "Sudoku not solved :-( " << endl;
		}
		cout << "Sanity: " << ((mySudoku.getSanity() == SANE) ? "SANE" : "INSANE")  << endl;
		cout << "Hits: " << this->hits << endl;
		cout << "Iterations" << this->iterations << endl;
		cout << "Gaps: " << mySudoku.getGaps() << endl;
		cout << "Two chances: " << this->twoChances << endl;	
	}





	

	/************************
	*       SOLVE funktions
	*     
	*  solve () - general Solver, delegates accordingly
	*  solveAlternate()  - Alternates between ExcludeNumbers and excludeFields 
	*/

	void solve() {



		cout << "Trying to solve with exclusive field / exclusive value method." << endl; 
		// First try solving with findField / findValue method.
    	this->solveAlternate(false);
		if (mySudoku.isSolved()) {
			cout << "Could Solve with find Value / find Field methods... this Sudoku was pretty easy. Is that all you got? :-) " << endl;
			return;
		}
		else {
			cout << "failed." << endl;
		}

		cout << endl << "Trying shallow backtracking approach.. (still fast, but doesn't solve anything)" << endl;
		// When that fails we try the shallow backtracking approach, still pretty fast, but doesnt solve anything.
		this->solveBacktrackingShallow();
		this->mySudoku.sanityAll();
		if (mySudoku.isSolved()) {
			cout << "Could Solve with shallow backtracking... this Sudoku was pretty easy. Is that all you got? :-) " << endl;
			return;
		}
		else {
			cout << "failed." << endl;
		}

		cout << endl <<  "Starting deep backtracking approach... should solve almost anything" << endl;
		// When nothing helps ... we use pure force ... that'll help
		solveBacktrackWraper();
		this->mySudoku.sanityAll();
		if (mySudoku.isSolved()) {
			cout << "phew ... you almost got me with that.. That was kind of a hassle. But try someting harder if you like. " << endl;
			return;
		}
		else {
			cout << "Failed! " << endl << "That means there wasn't always a field that only had two candidate numbers... " << endl;
			cout << "This case did never happen so far, even with the worst nightmares of Sudokus..." << endl;
		}
	}


	void solveBacktrackWraper() {
		cout << "Starting Backtrack deep! ";
		std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
		std::pair<int, Sudoku>  result = solveBacktrackDeep();
		std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

		cout << "Backtrack deep ended in " << (duration / 1000000) << "  seconds with result " << result.first << endl;
		if (result.first == 1) {
			cout << " Sudoku Soled !!!" << endl;
			this->mySudoku = result.second;
		}
		else {
			cout << " Sudoku not solved :-( :-( ";
			if (result.first == -1) {
				cout << "Sudoku INSANE. Possibly insane from the start?? " << endl;
			}
			else if (result.first == -2) {
				cout << " There were no Fields with two candidates left...  Maybe check candidates with three candidates as well??" << endl;
			}

		}
	}


	// Iterating over Exclude numbers and exclude RowCol 
	// as long as new hits are made.
	void solveAlternate(bool doSanityCheck) {
		int oldHits = -1;
		int alternations = 0;
		while (mySudoku.getGaps() > 0) {
			alternations++;
			oldHits = this->hits;
			solveExcludeNumbers(doSanityCheck);
			if (oldHits == this->hits) {
				if (this->verbosity >= 2) cout << "Exclude numbers messed up hits after " << alternations << " iterations" << endl;
				break;
			}

			oldHits = this->hits;
			solveExcludeFields();
			if (oldHits == this->hits) {
				if (this->verbosity >= 2) cout << "Exclude colRow messed up, no new hits hits after " << alternations << " iterations" << endl;
				break;
			}
		}
	}


	/**************************************
	**									 **
	**            EXCLUDE Numbers		 **
	**									 **
	***************************************/

	// solve by exclude Numbers wraper
	// Iterates over the real method, as long as it still makes hits.
	void solveExcludeNumbers(bool doSanityCheck) {
		int oldHits = -1;
		while (mySudoku.getGaps() > 0 && (oldHits != this->hits)) {
			oldHits = this->hits;
			solveExcludeNumbersIteration(doSanityCheck);
		}
	}


	// Solve Exclude Numbers Iteration
	// This checks excludes candidateNumbers for a field by checking,
	// if they are alreary in the same row, column or sector.
	void solveExcludeNumbersIteration(bool doSanityCheck) {
		//int fullRange[] = 

		this->iterations++;
		for (int ix = 0; ix <= maxCoord; ix++) {
			for (int iy = 0; iy <= maxCoord; iy++) {

				Coord gap = Coord(ix, iy);
				if (this->mySudoku.getField(gap) == 0) {


					int result = solveExcludeNumbersCoord(gap);
					// Negative for failure!
					if (result > 0) {
						mySudoku.assign(gap, result);
					}

					bool solved = (mySudoku.isSolved());
					if (solved) {
						//Solved here.
						return;
					}

					if (doSanityCheck) {
						this->mySudoku.sanityCoord(gap);
						if (mySudoku.getSanity() == INSANE)
						{
							// Insane here
							return;
						}
					}

				}
			}
		}

	}


	// This tries to find a certain value for the given coord
	// (by excluding candidateNumbers in row, column and sector) 
	int solveExcludeNumbersCoord(Coord gap) {

		std::vector<char> row = mySudoku.getRow(gap.y);
		std::vector<char> col = mySudoku.getCol(gap.x);
		std::vector<char> sec = mySudoku.getSector(gap);


		std::vector<int> all;
		all.insert(all.end(), row.begin(), row.end());
		all.insert(all.end(), col.begin(), col.end());
		all.insert(all.end(), sec.begin(), sec.end());

		// Copy the full range set.
		std::set<int> _workRange = fullrange;

		for (auto const& val : all) {
			if (_workRange.size() == 1) break;
			_workRange.erase(val);
		}

		set<int>::iterator it;

		if (_workRange.size() == 1) {
			it = _workRange.begin();
			int result = *it;
			//cout << "Coord" << gap.toStr();
			//cout << " found unique " << result << endl;
			this->hits++;
			this->hitsMap.insert(std::make_pair(gap, result));
			//this->mySudoku[gap.y][gap.x] = result;
			// Remove key from candidates map, if it was there ...
			candidates.erase(Coord(gap.x, gap.y));
			candidatesDoubles.erase(Coord(gap.x, gap.y));
			return result;

		}
		else if (_workRange.size() <= this->CANDIDATES_MAX) {
			if (_workRange.size() == 2) {
				candidatesDoubles.insert(std::make_pair(gap, _workRange));

			}
			this->twoChances++;
			// Inserting candidates into map.
			candidates.insert(std::make_pair(gap, _workRange));

		}
		else {
			/*cout << "multiple vals: ";
			for (it = remain_nums.begin(); it != remain_nums.end(); ++it) {
			int ans = *it;
			cout << ans << endl;
			}*/
		}
		return -1;
	}



	/**************************************
	**									 **
	**            exclude FIELDS		 **
	**									 **
	***************************************/

	void solveExcludeFields() {
		int oldHits = -1;
		while (mySudoku.getGaps() > 0) {
			oldHits = this->hits;
			solveExcludeFieldsIteration();

			// When no more hits were made, break the loop
			if (oldHits == this->hits) break;
		}

		if (mySudoku.isSolved()) {
			if (this->verbosity >= 1) cout << "SOLVED by Exclude ColRow" << endl;
		}
	}

	// Looks for Rows, and excludes places for numbers to go.
	void solveExcludeFieldsIteration() {
		//set < pair<Coord, int>> fields_sector_free;
		map<Coord, int> fields_sector_all;
		map<Coord, int> fields_sector_free;
		map<Coord, int> fields_sector_ocu;

		set<int> rows_tmp = { 0,1,2 };
		set<int> cols_tmp = { 0,1,2 };

		// Loop Sectors 0 - 8 
		this->iterations++;
		for (int iSec = 0; iSec <= Sudoku::RANGE_MAX; iSec++) {
			fields_sector_all.clear();
			fields_sector_free.clear();
			fields_sector_ocu.clear();

			Sector sec = Sector(iSec);
			std::vector<Coord> coords_in_sector = sec.coordsInSector();



			// Split up free and occupied fields with coords_in_sector.
			fields_sector_all = mySudoku.getVectorWithCoords(coords_in_sector);  //getSector with coords now unused
			for (auto const& field : fields_sector_all) {
				if (field.second == 0) {
					fields_sector_free.insert(field);
				}
				else {
					fields_sector_ocu.insert(field);
				}
			}

			// Loop through occupied fields, and drop those vals from remaining numbers.
			// So we have only the candidate numbers in that sector.
			std::set<int> remain_nums = fullrange;
			for (auto const& val : fields_sector_ocu) {
				if (remain_nums.size() == 1) break;
				remain_nums.erase(val.second);
			}


			// Getting the top left corner of the Sector..
			Coord sector_topLeft = sec.getCoord();

			set<int> rows_remain = rows_tmp;
			set<int> cols_remain = cols_tmp;

			set<int> row_having_num;
			set<int> col_having_num;


			// loop through Candidate numbers
			for (int const& candidateNum : remain_nums) {
				map<Coord, int> fields_sector_free_for_num = fields_sector_free;;

				// Excluding cols that have that number in them
				for (int col = 0; col < 3; col++) {
					int absolute_col = col + sector_topLeft.y;

					if (mySudoku.numIsInRow(absolute_col, candidateNum)) {
						row_having_num.insert(absolute_col);
					}

					//if (it != cols_remain.end()) ++it;
				}

				// Excluding rows that have that number in them
				//for (set<int>::iterator it = rows_remain.begin(); it != rows_remain.end(); ) {
				for (int row = 0; row < 3; row++) {
					//int row = *it;
					int absolute_row = row + sector_topLeft.x;

					if (mySudoku.numIsInCol(absolute_row, candidateNum)) {
						col_having_num.insert(absolute_row);
					}

					//if (it != rows_remain.end()) ++it;
				}

				//cout << endl <<  " Sector " << sec.toString() << " value: " << candidateNum << endl;




				for (auto const& y : row_having_num) {
					Coord delCord1 = Coord(sector_topLeft.x + 0, y);
					Coord delCord2 = Coord(sector_topLeft.x + 1, y);
					Coord delCord3 = Coord(sector_topLeft.x + 2, y);


					//cout << " Deleting Coords in row " << y << " " << delCord1.toStr() << delCord2.toStr() << delCord3.toStr() << endl;

					fields_sector_free_for_num.erase(delCord1);
					fields_sector_free_for_num.erase(delCord2);
					fields_sector_free_for_num.erase(delCord3);
				}
				row_having_num.clear();

				for (auto const& x : col_having_num) {
					Coord delCord1 = Coord(x, sector_topLeft.y + 0);
					Coord delCord2 = Coord(x, sector_topLeft.y + 1);
					Coord delCord3 = Coord(x, sector_topLeft.y + 2);

					//cout << " Deleting Coords in col " << x << " " << delCord1.toStr() << delCord2.toStr() << delCord3.toStr() << endl;
					fields_sector_free_for_num.erase(delCord1);
					fields_sector_free_for_num.erase(delCord2);
					fields_sector_free_for_num.erase(delCord3);

					// TODO Acutally delete Coord!
				}
				col_having_num.clear();



				// If there is only one free field left, that's ours
				if (fields_sector_free_for_num.size() == 1) {
					pair<Coord, int> lastFreeField = *fields_sector_free_for_num.begin();
					mySudoku.assign(lastFreeField.first, candidateNum);
					hitsMap.insert(make_pair(lastFreeField.first, candidateNum));
					candidates.erase(lastFreeField.first);
					candidatesDoubles.erase(lastFreeField.first);
					this->hits++;
					if (this->verbosity >= 1) cout << endl << " Sector " << sec.toString() << " value: " << candidateNum << endl;
					if (this->verbosity >= 1) cout << " found unique Field " << lastFreeField.first.toStr() << endl << endl;

				}
				else {
					//cout << " did not find unique Field :-(" << endl;
				}

			} // for candidate numbers




		}// for Sectors

	}




	/**************************************
	**									 **
	**           BACKTRACKERS     		 **
	**									 **
	***************************************/



	// This is going to be a deep backtracker some time.
	std::pair<int, Sudoku>  solveBacktrackDeep() {
		RecursionCounter<0> recursionDepth;
		recursiveIterations++;
		if(this->verbosity >= 1) cout << " Recursion level: " << recursionDepth << " Iterations: " << recursiveIterations << endl;

		// To gathe the Candidates
		solveAlternate(false);
		mySudoku.sanityAll();
		if (mySudoku.getSanity() == INSANE) {
			return make_pair(-1, mySudoku);
		}
		else {
			if (mySudoku.isSolved()) {
				return make_pair(1, mySudoku);
			}
			else {
				if (this->verbosity >= 1) cout << "We have " << candidatesDoubles.size() << " doubles" << endl;
				if (candidatesDoubles.size() == 0) {
					if (this->verbosity >= 1) cout << "No Doubles found :-( So have to return :-( " << endl;
					return make_pair(-2, mySudoku);
				}

				std::pair<Coord, std::set<int>> firstField2Candidates = *candidatesDoubles.begin();
				Coord gapCoord        =  firstField2Candidates.first;
				int   firstCandidate  = *firstField2Candidates.second.begin();
				int   secondCandidate = *std::next(firstField2Candidates.second.begin(), 1);

				if (this->verbosity >= 2) cout << " Coord " << gapCoord.toStr() << " is either " << firstCandidate << " or " << secondCandidate << endl;
				
				// Now copy the Sudoku into another one, and assign the first candidate to it.
				Sudoku sudokoWithFirst = Sudoku(mySudoku);
				sudokoWithFirst.assign(gapCoord, firstCandidate);
				
				// Not make a recursive instance of the class itself.
				SudokuSolver tmp_solverWithFirst = SudokuSolver(sudokoWithFirst);
				std::pair<int, Sudoku> solverWithFirstResult = tmp_solverWithFirst.solveBacktrackDeep();

				// When return value negative, it either went insane, or there are no more candidates
				if (solverWithFirstResult.first == 1) {
					// Return value is positive... Sudoku is solved!
					return solverWithFirstResult;
				}
				else if(solverWithFirstResult.first == -2) {
					// Sudoku has no more doubles (This is bad!)
					return solverWithFirstResult;
				}
				else if (solverWithFirstResult.first == -1) {
					// This means the first Candidate made the Sudoku insane... bad?
					// no! good! That means that we can be dead certain that secondCandidate value is correct!
					Sudoku sudokuWithSecond = Sudoku(mySudoku);
					sudokuWithSecond.assign(gapCoord, secondCandidate);
					
					SudokuSolver tmp_solverWithSecond = SudokuSolver(sudokuWithSecond);
					std::pair<int, Sudoku> solverWithSecondResult = tmp_solverWithSecond.solveBacktrackDeep();
					// No Checking needed... The Value inserted is *dead*certain 
					return solverWithSecondResult;
				}
				else {
					cout << " When you see this something really bad happened." << endl;;
					return solverWithFirstResult;
				}
			}
		}
		

	}


	// This uses Backtracking, but goes only in 1 ~inception~ recursion layer. 
	// Then tries normal methods, and checks for sanity.
	// If all other candidateNumbers make the Sudoku insane, this candidate is chosen as only leftover.
	void solveBacktrackingShallow() {
		if (this->verbosity >= 1) cout << " Sudoku not solved yet. Starting with candidates" << endl;
		int oldHits = -1;
		while ( !mySudoku.isSolved() && (oldHits != this->hits)) {
			oldHits = this->hits;
			this->iterations++;
			std::map<Coord, std::set<int>>::iterator itDoubles;
			for (auto const& aCandidate : candidates) {
			//itDoubles = candidates.begin();
			//while(itDoubles != candidates.end()){
			//for (itDoubles = candidates.begin(); itDoubles != candidates.end();){// ++itDoubles) {

				//std::pair<Coord, std::set<int>> aCandidate = *itDoubles;
				
				if (this->verbosity >= 1) cout << "for doubles" << endl;
				Coord gapCoord = aCandidate.first;

				// Check whether the field is still a gap...
				//if (mySudoku[gapCoord.y][gapCoord.x] == 0) {

				std::set<int> candidateNumbers = aCandidate.second;
				std::set<int>::iterator itCandidates;

				for (itCandidates = candidateNumbers.begin(); itCandidates != candidateNumbers.end(); ){ // ++itCandidates) {
					if (this->verbosity >= 2) cout << "for candidates" << endl;
					int candidate = *itCandidates;
					//itCandidates++;

					if (this->verbosity >= 2) cout << "Coords " << gapCoord.x << "/" << gapCoord.y << " Candidate " << candidate << endl;
					// Copying current sudokuSolver1
					Sudoku tmp_sudoku = this->mySudoku;

					// Inserting candidate into temp sudokuSolver1... Then proceed and check if fails.
					//tmp_sudoku[gapCoord.y][gapCoord.x] = candidate;
					tmp_sudoku.assign(gapCoord, candidate);
					SudokuSolver tmpSolver = SudokuSolver(tmp_sudoku);
					if (this->verbosity >= 3) cout << "---------------------------tmp" << endl;
					tmpSolver.solveAlternate(false);
					tmpSolver.mySudoku.sanityAll();
					if (this->verbosity >= 3) cout << "/--------------------------tmp" << endl;

					if (tmpSolver.mySudoku.getSanity()   == INSANE) {
						if (this->verbosity >= 1) cout << "Sudoku insane with  " << candidate << " at posision " << gapCoord.toStr() << " in " << tmpSolver.iterations << " iterations " << endl;
						itCandidates = candidateNumbers.erase(itCandidates); // Remove from Candidates.
						//itCandidates = candidateNumbers.begin();
					}
					else {
						++itCandidates;
					}
				}

				// When there is only one left, we put the number in the sudokuSolver1!
				if (candidateNumbers.size() == 1) {
					itCandidates = candidateNumbers.begin();
					int leftOver = *itCandidates;
					mySudoku.assign(gapCoord, leftOver); // [gapCoord.y][gapCoord.x] = leftOver;
					hitsMap.insert(make_pair(gapCoord, leftOver));
					hits++;
						
					//itDoubles =
						//candidates.erase(gapCoord);
					//itDoubles = candidates.begin();
					this->hitsMap.insert(std::make_pair(gapCoord, leftOver));
						
					//this->solveExcludeNumbers(false);
					// Redo solve Bf? 
				}
				else {
					// Important! Must be REACHABLE !!!
					//++itDoubles;
					if (this->verbosity >= 3) cout << "more than one candidate left! " << endl;
				}
			
			}

			this->solveExcludeNumbers(true);
		}

	}

	


	


};



/**************************************
**									 **
**            Main					 **
**									 **
***************************************/

int main(int argc, char* argv[])
{
	/*
	*   must be called like solver(.exe) --file SudokuTelegraph.csv
	*   example csv files are delivered with the executable.
	*/

	string filename;
	if (argc == 3) {
		if (std::string(argv[1]) == "--file") {
		
			ifstream my_file(argv[2]);
			if (my_file.good())
			{
				filename = std::string(argv[2]);
				cout << "Using File: " << filename << endl;
			}
			else {
				cout << "File " << argv[2] << " is not readable!" << endl;
				return -1;
			}
			
		}
		else {
			cout << "Unknown parameter!" << endl;
			cout << "'" << argv[1] << "' " << argv[2] << endl;
			return -1;
		}
	}
	else {
		cout << "Usage: --file csvFile" << argc << endl;
		cout << "e.g. --file SudokuTelegraph.csv for a hard one." << filename << endl;
		return 0;
	}
	

	

	SudokuSolver sudokuSolver1 =  SudokuSolver(filename);
	sudokuSolver1.setVerbosity(0);
	cout << "Using Following Sudoku:\n";
	sudokuSolver1.print();
	cout << endl << endl << endl;
	cout << "Starting solver " << endl;
	sudokuSolver1.solve();


	cout << endl << endl << endl;
	cout << "------------------------------main() -> print --" << endl;
	sudokuSolver1.print();

	sudokuSolver1.printCandidateDoubles();


	return 0;
}


