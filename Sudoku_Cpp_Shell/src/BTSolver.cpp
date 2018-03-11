#include"BTSolver.hpp"
#include <climits>

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
	ConstraintNetwork::VariableSet variables = network.getVariables();
	ConstraintNetwork::VariableSet mVariables;
	for ( Variable* var: variables ) {
		if ( var->isModified() ) {
			mVariables.push_back( var );
			var->setModified( false );
		}
	}

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

	    // ConstraintNetwork::ConstraintRefSet varRelatedConstraints = network.getConstraintsContainingVariable( var );
	    // for ( Constraint* c : varRelatedConstraints )
	    // 	if ( !c->isConsistent() )
	    // 		return false;
			if ( !network.isConsistent() ) return false;
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
 * Return: true if assignment is consistent, false otherwise
 */

bool BTSolver::norvigCheck ( void )
{
	// (1) If a variable is assigned then eliminate that value from
	// the variable's neighbours: Forward Checking
	// ConstraintNetwork::VariableSet variables = network.getVariables();
	ConstraintNetwork::VariableSet variables = network.getVariables();
	ConstraintNetwork::VariableSet mVariables;
	for ( Variable* var: variables ) {
		if ( var->isModified() ) {
			mVariables.push_back( var );
			var->setModified( false );
		}
	}
	for ( Variable* var: mVariables ) {
		// for each variable
		if ( var->getDomain().isEmpty() ) return false;
		if ( var->isAssigned() ) {
			// if the variable is assigned, eliminate the value from its neighbours
			ConstraintNetwork::VariableSet neighbours = network.getNeighborsOfVariable( var );
			// find the the value assigned
			int val = var->getAssignment();
			// for each neighbour of this variable
			for ( Variable* n: neighbours ) {
				Domain nDomain = n->getDomain();
				if ( nDomain.contains( val ) ) {
					if ( n->isAssigned() ) return false;
					trail->push(n);
					// remove the value from var's domain
					n->removeValueFromDomain( val );
				}
			}
			if ( !network.isConsistent() ) return false;
		}
	}


	// (2) If a constraint has only one possible place for a value
	// then put the value there
	ConstraintNetwork::ConstraintSet constraints = network.getConstraints();
	for ( Constraint constraint: constraints ) {
		// initialize an array counting # of occurrences for each value
		vector<int> count;
		count.resize( sudokuGrid.get_n(), 0 );
		// for each variable in the constraint, find its domain, increment the
		// count for each value in the domain by 1
		Constraint::VariableSet constraintVars = constraint.vars;
		for ( Variable* var: constraintVars ) {
			for ( int val: var->getDomain() )
				count[val-1]++;
		}
		// iterate through the count vector: if count = 1, figure out the variable,
		// trail push and make assignment
		for ( int i = 0; i < count.size(); i++ ) {
			if ( count[i] == 1 ) {
				// make the assignment
				for ( Variable* var: constraintVars )
					if ( var->getDomain().contains( i+1 ) ) {
						// trail push before assignment
						trail->push( var );
						var->assignValue( i+1 );
					}
			}
		}
	}
	// Check consistency of the network
	bool isConsistent = network.isConsistent();
	// vector<string> output;
	// output.push_back("false: not consistent");
	// output.push_back("true: consistent");
	// cout << output[isConsistent] << endl;
	return isConsistent;
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

//self writen compare function
bool comparePtrToVariable(Variable* a, Variable* b) { return (a->size() < b->size()); }

Variable* BTSolver::getMRV ( void )
{
	//get all the variables
	ConstraintNetwork::VariableSet variables = network.getVariables();

	//get all the unassigned variables
	ConstraintNetwork::VariableSet unassigned;
	for ( Variable* v : variables)
		if ( !(v->isAssigned()) )
			unassigned.push_back( v );

  //check if all values are assigned? or maybe some unpredictable errors occur
  if ( unassigned.size() == 0)
	   return nullptr;
/*
	Variable* minVar = unassigned[0];
	for ( Variable *v : unassigned ) {
		if ( v->size() < minVar->size() ) {
			minVar = v;
		}
	}

  return minVar;
*/
	std::sort(unassigned.begin(), unassigned.end(), comparePtrToVariable);
	return unassigned[0];
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

	// Find unassigned variables
	ConstraintNetwork::VariableSet variables = network.getVariables();

	ConstraintNetwork::VariableSet unassigned;
	for ( Variable *v : variables)
		if ( !v->isAssigned() )
			unassigned.push_back( v );

	// If all values have been assigned, return nullptr
	if ( unassigned.size() == 0 )
		return nullptr;

	// return unassigned[0];
	// For each variable, count the number of their unassigned neighbors
	int maxDeg = -1;
	Variable *maxDegVar = nullptr;
	for ( Variable *v : unassigned ) {
		ConstraintNetwork::VariableSet neighbors = network.getNeighborsOfVariable( v );

		int neighborCount = 0;
		for ( Variable *vv: neighbors )
			if ( !vv->isAssigned() )
				neighborCount++;
		/*
		ConstraintNetwork::ConstraintRefSet outset = network.getConstraintsContainingVariable( v );
		int neighborCount = outset.size();
		*/
		if ( neighborCount > maxDeg ) {
			maxDeg = neighborCount;
			maxDegVar = v;
		}
	}
	//if (maxDegVar == nullptr) cout << "Error!\n";
	return maxDegVar;
	/*

	map<Variable*, int> countMap;

	ConstraintNetwork::VariableSet variables = network.getVariables();
    for ( Variable* var : variables)
    {
    	//we only take unassigned vars
    	if ( !(var->isAssigned()) )
    	{
    		    	//initialize var count = 0
    	countMap.insert(std::pair<Variable*, int>(var, 0));

    	//get all the neighbours
	    ConstraintNetwork::VariableSet neighbors = network.getNeighborsOfVariable( var );

		//for each neighbour, count occurence of the values v has, the more counts, the more constrained
		for ( Variable* neigh : neighbors )
		    if ( !(neigh->isAssigned()) )
		    	countMap[var] += 1;
    	}


    }

    // it means all assigned, or maybe some errors
    if ( countMap.size() == 0)
    	return nullptr;
	//sort the map

	//reverse key-value in a vector, so as to sort by map's value, which is vector's key
    vector<std::pair<int, Variable*>> reverseMap;
    for (std::map<Variable*, int>::iterator it=countMap.begin(); it!=countMap.end(); ++it)
    {
    	std::pair<int, Variable*> tmp;
    	tmp.first = it->second;
    	tmp.second = it->first;
    	reverseMap.push_back(tmp);
    }

    //sort
	std::sort(reverseMap.begin(), reverseMap.end());

	return reverseMap.back().second;

*/
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
	//get all the variables
	ConstraintNetwork::VariableSet variables = network.getVariables();

	//get all the unassigned variables
	ConstraintNetwork::VariableSet unassigned;
	for ( Variable* v : variables)
		if ( !v->isAssigned() )
			unassigned.push_back( v );

  //check if all values are assigned? or maybe some unpredictable errors occur
  if ( unassigned.size() == 0)
	   return nullptr;
/*
  std::sort(unassigned.begin(), unassigned.end(), comparePtrToVariable);
	Variable* minVar = unassigned[0];
	for ( int i = unassigned.size() - 1; i >= 0; i-- ) {
		if (unassigned[i]->size() > minVar->size())
			unassigned.pop_back();
		else break;
	}
*/
	if (unassigned.size() == 1)
		return unassigned[0];

	// Find minVar
	Variable *minVar = unassigned[0];
	int minCount = 0;
	for ( Variable *v: unassigned )
		if ( v->size() < minVar->size() ) {
			minVar = v;
			minCount++;
		}
	if (minCount < 2) return minVar;

	// For each min variable, count the number of their unassigned neighbors
	int maxDeg = -1;
	Variable *maxDegVar = minVar;
	for ( Variable *v : unassigned ) {
		if ( v->size() == minVar->size() ) {
			int neighborCount = 0;
			ConstraintNetwork::VariableSet neighbors = network.getNeighborsOfVariable( v );

			for ( Variable *vv: neighbors ) {
				if ( !vv->isAssigned() )
					neighborCount++;
			}
			if ( neighborCount > maxDeg ) {
				maxDeg = neighborCount;
				maxDegVar = v;
			}
		}
	}
	return maxDegVar;
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

	//for each neighbour, count occurrence of the values v has, the more counts, the more constrained
	for ( Variable* neigh : neighbors )
	{
		Domain::ValueSet neighValues = neigh->getValues();
		for ( int domainVal : neighValues )
			if ( countMap.count(domainVal) > 0 )
				countMap[domainVal] += 1;
	}

	//sort the map

	//reverse key-value in a vector, so as to sort by map's value, which is vector's key
    vector<std::pair<int, int>> reverseMap;
    for (std::map<int,int>::iterator it=countMap.begin(); it!=countMap.end(); ++it)
    {
    	std::pair<int, int> tmp;
    	tmp.first = it->second;
    	tmp.second = it->first;
    	reverseMap.push_back(tmp);
    }

    //sort
	std::sort(reverseMap.begin(), reverseMap.end());

    // prepare returning vector
    vector<int> rtn;

	// Iterate over the vector of pairs using range base for loop
	for (std::pair<int, int> element : reverseMap )
		rtn.push_back(element.second);

	return rtn;
}


vector<int> BTSolver::getTournVal ( Variable* v )
{
	return vector<int>();
}

// =====================================================================
// Engine Functions
// =====================================================================

void BTSolver::solve ( void )
{
	//cout << "                                     #1" << endl;
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
	//cout << v->getName() << " r: " << v->row() << " c: " << v->col() << " b: " << v->block() << endl;


//cout << "                                     #2" << endl;
	// Attempt to assign a value
	for ( int i : getNextValues( v ) )
	{
		//cout << "try value " << i << " :";
		// Store place in trail and push variable's state on trail
		trail->placeTrailMarker();
		trail->push( v );

		// Assign the value
		v->assignValue( i );
		// Propagate constraints, check consistency, recurse
		if ( checkConsistency() ) {
			/*int count = 0;
			for ( Variable* var : network.getVariables() )
			{
				count += 1;
					cout << var->getAssignment();
					if ( count % 4 == 0)
						cout << endl;
			}
			cout << endl;*/
			solve();
		}

		// If this assignment succeeded, return
		if ( hasSolution )
			return;

		// Otherwise backtrack
		trail->undo();
	}
	//cout << "                                     #3" << endl;
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
