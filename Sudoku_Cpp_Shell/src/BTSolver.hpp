#ifndef BTSOLVER_HPP
#define BTSOLVER_HPP

#include "SudokuBoard.hpp"
#include "Domain.hpp"
#include "Variable.hpp"
#include "ConstraintNetwork.hpp"

#include <stack>
#include <utility>
#include <iostream>
#include <vector>

class BTSolver
{
public:
	// Constructor
	BTSolver ( SudokuBoard board, std::string val_sh, std::string var_sh, std::string cc );

    // CHECKING
	bool checkConsistency ( void );
	// Consistency Checks (Implement these)
	bool assignmentsCheck ( void );
	bool forwardChecking  ( void );
	bool norvigCheck      ( void );

    // SELECTING
    //this gives back a variable, according to MRV, Deg or MRV+TB
	//if not specified, then silly method is used
	Variable* selectNextVariable ( void );
	// Variable Selectors (Implement these)
	//getfirst is a silly method, and we don't need to impement this selector
	Variable* getfirstUnassignedVariable ( void );
	Variable* MinimumRemainingValue      ( void );
	Variable* Degree                     ( void );
	Variable* MRVwithTieBreaker          ( void );


    // GET VALUE
	std::vector<int> getNextValues ( Variable* v );
	// Value Selectors (Implement these)
	//for each variables in the network, check each if all of its neighbors are consistent
	std::vector<int> getValuesInOrder       ( Variable* v );
	std::vector<int> LeastConstrainingValue ( Variable* v );

	// Engine Functions
	void solve ( int level = 0 );

	

	// Helper Functions
	bool haveSolution ( void );
	SudokuBoard getSolution ( void );
	ConstraintNetwork getNetwork ( void );

private:
	ConstraintNetwork network;
	std::stack<std::pair<Variable*, Domain> > trail;
	bool hasSolution = false;
	SudokuBoard sudokuGrid;

	std::string varHeuristics;
	std::string valHeuristics;
	std::string cChecks;
};


#endif