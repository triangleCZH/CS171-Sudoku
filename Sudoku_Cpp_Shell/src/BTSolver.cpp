#include"BTSolver.hpp"

using namespace std;

// =====================================================================
// Constructors
// =====================================================================

BTSolver::BTSolver ( SudokuBoard input, Trail* _trail,  string val_sh, string var_sh, string cc )
: sudokuGrid( input.get_p(), input.get_q(), input.get_board() ), network( input )
{
	valHeuristics = val_sh;
	varHeuristics = var_sh;
	cChecks =  cc;

	trail = _trail;
}

// =====================================================================
// Consistency Checks
// =====================================================================

// Basic consistency check, no propagation done
bool BTSolver::assignmentsCheck ( void )
{
	for ( Constraint c : network.getConstraints() )
		if ( ! c.isConsistent() )
			return false;

	return true;
}

/**
 * Part 1 TODO: Implement the Forward Checking Heuristic
 *
 * This function will do both Constraint Propagation and check
 * the consistency of the network
 *
 * (1) If a variable is assigned then eliminate that value from
 *     the square's neighbors.
 *
 * Note: remember to trail.push variables before you assign them
 * Return: true is assignment is consistent, false otherwise
 */

/*no matter I call variable remove or domain remove, no matter the domain decreased
variable is considered as modified or not, once it has only one value, it is considered
as assgigned, then if a row is [1,2] [1,2] [assign = 1], they both get 2 and think
themselves assigned*/
bool BTSolver::forwardChecking ( void )
{   //TODO: maybe don;t use getConstraint, but go get modified variables and get their neighbors
	//check the current top of the stack, get the variable and its new value
	ConstraintNetwork::VariableSet mVariables = network.getModifiedVariables();
	for ( Variable* var: mVariables ) 
	{
		Domain varDomain = var->getDomain();
		// Uncertain where this error will be, but whenever empty domains found, it's errorness
		if ( varDomain.size() == 0 ) return false;
		// For forwardChecking, I don't care about those modified through forward checking, but only those just assigned
		if ( varDomain.size() != 1 ) continue;

		// this returns int
		int varAssignment = var->getAssignment();

		// iterate on each neighbor of this variable
	    ConstraintNetwork::VariableSet neighbors = network.getNeighborsOfVariable( var );
	    for ( Variable* neigh: neighbors)
	    {
	    	Domain neighDomain = neigh->getDomain();
	    	// if doesn't contain, then not affected
	    	if ( neighDomain.contains( varAssignment )) 
	    	{
	    		// if it contains this in domain, and even assigned with varAssignment, then inconsistent
	    		if ( neigh->isAssigned() ) return false;
	    		//first back up this neighbor's domain in trail stack
	    		trail->push( neigh );
	    		neigh->removeValueFromDomain( varAssignment );
	    	}
	    }
	    // now we check if inconsistency happens after domain update
	    ConstraintNetwork::ConstraintRefSet varRelatedConstraints = network.getConstraintsContainingVariable( var );
	    for ( Constraint* c : varRelatedConstraints ) 
	    	if ( !c->isConsistent() ) 
	    		return false;
	}
	return true;
}

/**
 * Part 2 TODO: Implement both of Norvig's Heuristics
 *
 * This function will do both Constraint Propagation and check
 * the consistency of the network
 *
 * (1) If a variable is assigned then eliminate that value from
 *     the square's neighbors.
 *
 * (2) If a constraint has only one possible place for a value
 *     then put the value there.
 *
 * Note: remember to trail.push variables before you assign them
 * Return: true is assignment is consistent, false otherwise
 */
bool BTSolver::norvigCheck ( void )
{
	return false;
}

/**
 * Optional TODO: Implement your own advanced Constraint Propagation
 *
 * Completing the three tourn heuristic will automatically enter
 * your program into a tournament.
 */
bool BTSolver::getTournCC ( void )
{
	return false;
}

// =====================================================================
// Variable Selectors
// =====================================================================

// Basic variable selector, returns first unassigned variable
Variable* BTSolver::getfirstUnassignedVariable ( void )
{
	for ( Variable* v : network.getVariables() )
		if ( !(v->isAssigned()) )
			return v;

	// Everything is assigned
	return nullptr;
}

/**
 * Part 1 TODO: Implement the Minimum Remaining Value Heuristic
 *
 * Return: The unassigned variable with the smallest domain
 */
Variable* BTSolver::getMRV ( void )
{
	return nullptr;
}

/**
 * Part 2 TODO: Implement the Minimum Remaining Value Heuristic
 *                with Degree Heuristic as a Tie Breaker
 *
 * Return: The unassigned variable with, first, the smallest domain
 *         and, second, the most unassigned neighbors
 */
Variable* BTSolver::getDegree ( void )
{
	return nullptr;
}

/**
 * Part 2 TODO: Implement the Minimum Remaining Value Heuristic
 *                with Degree Heuristic as a Tie Breaker
 *
 * Return: The unassigned variable with the smallest domain and involved
 *             in the most constraints
 */
Variable* BTSolver::MRVwithTieBreaker ( void )
{
	return nullptr;
}

/**
 * Optional TODO: Implement your own advanced Variable Heuristic
 *
 * Completing the three tourn heuristic will automatically enter
 * your program into a tournament.
 */
Variable* BTSolver::getTournVar ( void )
{
	return nullptr;
}

// =====================================================================
// Variable Selectors
// =====================================================================

// Default Value Ordering
vector<int> BTSolver::getValuesInOrder ( Variable* v )
{
	vector<int> values = v->getDomain().getValues();
	sort( values.begin(), values.end() );
	return values;
}

/**
 * Part 1 TODO: Implement the Least Constraining Value Heuristic
 *
 * The Least constraining value is the one that will knock the most
 * values out of it's neighbors domain.
 *
 * Return: A list of v's domain sorted by the LCV heuristic
 *         The LCV is first and the MCV is last
 */
// Since we will do consistencyCheck and remove values from domain,
// this new assignment should be consistent
vector<int> BTSolver::getValuesLCVOrder ( Variable* v )
{
	//<value, how many unassigned neighbors have this value>
	map<int, int> countMap; 
	Domain::ValueSet valueSet = v->getDomain().getValues();

	//initialize the counter 
	for ( int value : valueSet )
		countMap.insert(std::pair<int, int>(value, 0));

    //get all the neighbours
	ConstraintNetwork::VariableSet neighbors = network.getNeighborsOfVariable( v );

	//for each neighbour, count occurence of the values v has, the more counts, the more constrained
	for ( Variable* neigh : neighbors ) 
	{
		Domain::ValueSet neighValues = neigh->getValues();
		for ( int domainVal : neighValues ) 
			if ( countMap.count(domainVal) > 0 ) 
				countMap[domainVal] += 1;
	}

	//sort the map
    vector<std::pair<int, int>> reverseMap;
    for (std::map<int,int>::iterator it=countMap.begin(); it!=countMap.end(); ++it)
    {
    	std::pair<int, int> tmp;
    	tmp.first = it->second;
    	tmp.second = it->first;
    	reverseMap.push_back(tmp);
    }
    
    /*for ( std::pair<int, int> element : countMap )
    	reverseMap.push_back(std::pair<element.second, element.first>);*/

	// Defining a lambda function to compare two pairs. It will compare two pairs using second field
	/*auto cmp = 
			[](std::pair<int, int> elem1 ,std::pair<int, int> elem2)
			{
				return elem1.first < elem2.first;
			};*/

	// Declaring a set that will store the pairs using above comparision logic
	std::sort(reverseMap.begin(), reverseMap.end());

    // prepare returning vector
    vector<int> rtn;

	// Iterate over a set using range base for loop
	// It will display the items in sorted order of values
	for (std::pair<int, int> element : reverseMap )
		rtn.push_back(element.second);

	return rtn;
}

/**
 * Part 1 TODO: Implement the Least Constraining Value Heuristic
 *
 * The Least constraining value is the one that will knock the most
 * values out of it's neighbors domain.
 *
 * Return: A list of v's domain sorted by the LCV heuristic
 *         The LCV is first and the MCV is last
 */
vector<int> BTSolver::getTournVal ( Variable* v )
{
	return vector<int>();
}

// =====================================================================
// Engine Functions
// =====================================================================

void BTSolver::solve ( void )
{
	if ( hasSolution )
		return;

	// Variable Selection
	Variable* v = selectNextVariable();

	if ( v == nullptr )
	{
		for ( Variable* var : network.getVariables() )
		{
			// If all variables haven't been assigned
			if ( ! ( var->isAssigned() ) )
			{
				cout << "Error" << endl;
				return;
			}
		}

		// Success
		hasSolution = true;
		return;
	}

	// Attempt to assign a value
	for ( int i : getNextValues( v ) )
	{
		// Store place in trail and push variable's state on trail
		trail->placeTrailMarker();
		trail->push( v );

		// Assign the value
		v->assignValue( i );

		// Propagate constraints, check consistency, recurse
		if ( checkConsistency() )
			solve();

		// If this assignment succeeded, return
		if ( hasSolution )
			return;

		// Otherwise backtrack
		trail->undo();
	}
}

bool BTSolver::checkConsistency ( void )
{
	if ( cChecks == "forwardChecking" )
		return forwardChecking();

	if ( cChecks == "norvigCheck" )
		return norvigCheck();

	if ( cChecks == "tournCC" )
		return getTournCC();

	return assignmentsCheck();
}

Variable* BTSolver::selectNextVariable ( void )
{
	if ( varHeuristics == "MinimumRemainingValue" )
		return getMRV();

	if ( varHeuristics == "Degree" )
		return getDegree();

	if ( varHeuristics == "MRVwithTieBreaker" )
		return MRVwithTieBreaker();

	if ( varHeuristics == "tournVar" )
		return getTournVar();

	return getfirstUnassignedVariable();
}

vector<int> BTSolver::getNextValues ( Variable* v )
{
	if ( valHeuristics == "LeastConstrainingValue" )
		return getValuesLCVOrder( v );

	if ( valHeuristics == "tournVal" )
		return getTournVal( v );

	return getValuesInOrder( v );
}

bool BTSolver::haveSolution ( void )
{
	return hasSolution;
}

SudokuBoard BTSolver::getSolution ( void )
{
	return network.toSudokuBoard ( sudokuGrid.get_p(), sudokuGrid.get_q() );
}

ConstraintNetwork BTSolver::getNetwork ( void )
{
	return network;
}
