#include "BTSolver.hpp"
#include "SudokuBoard.hpp"
#include "Trail.hpp"

#include <iostream>
#include <ctime>
#include <cmath>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

using namespace std;

/**
 * Main driver file, which is responsible for interfacing with the
 * command line and properly starting the backtrack solver.
 */

int main ( int argc, char *argv[] )
{
	// Set random seed
	srand( time ( NULL ) );

	// Important Variables
	string file   = "";
	string var_sh = "";
	string val_sh = "";
	string cc     = "";

	for ( int i = 1; i < argc; ++i )
	{
		string token = argv[i];

		if ( token == "MRV" )
			var_sh = "MinimumRemainingValue";

		else if ( token == "DEG" )
			var_sh = "Degree";

		else if ( token == "MAD" )
			var_sh = "MRVwithTieBreaker";

		else if ( token == "LCV" )
			val_sh = "LeastConstrainingValue";

		else if ( token == "FC" )
			cc = "forwardChecking";

		else if ( token == "NOR" )
			cc = "norvigCheck";

		else if ( token == "TOURN" )
		{
			 var_sh = "tournVar";
			 val_sh = "tournVal";
			 cc     = "tournCC";
		}

		else
			file = token;
	}

	Trail trail;

	if ( file == "" )
	{
		SudokuBoard board( 3, 3, 7 );
		cout << board.toString() << endl;

		BTSolver solver = BTSolver( board, &trail, val_sh, var_sh, cc );

		solver.solve();

		if ( solver.haveSolution() )
		{
			cout << solver.getSolution().toString() << endl;
			cout << "Assignments: " << trail.getPushCount() << endl;
			cout << "Backtracks: "  << trail.getUndoCount() << endl;
		}
		else
		{
			cout << "Failed to find a solution" << endl;
		}

		return 0;
	}

	struct stat path_stat;
	stat ( file.c_str(), &path_stat );
	bool folder = S_ISDIR ( path_stat.st_mode );

	if ( folder )
	{
		DIR *dir;
		if ( ( dir = opendir ( file.c_str() ) ) == NULL )
		{
			cout << "[ERROR] Failed to open directory." << endl;
			return 0;
		}

		struct dirent *ent;

		int numSolutions = 0;
		while ( ( ent = readdir (dir) ) != NULL )
		{
			if ( ent->d_name[0] == '.' )
				continue;

			cout << "Running board: " << ent->d_name << endl;

			string individualFile = file + "/" + ent->d_name;


			SudokuBoard board( individualFile );

			BTSolver solver = BTSolver( board, &trail, val_sh, var_sh, cc );

			solver.solve();

			if ( solver.haveSolution() )
				numSolutions++;

			trail.clear();
		}

		cout << "Solutions Found: " << numSolutions << endl;
		cout << "Assignments: " << trail.getPushCount() << endl;
		cout << "Backtracks: "  << trail.getUndoCount() << endl;
		closedir (dir);

		return 0;
	}

	SudokuBoard board( file );
	cout << board.toString() << endl;

	BTSolver solver = BTSolver( board, &trail, val_sh, var_sh, cc );
	/*==================================
			map<Variable*, int> countMap; 
			cout << "test1" << endl;
			
			ConstraintNetwork::VariableSet variables = solver.getNetwork().getVariables();
			cout << "test2" << endl;
		    for ( Variable* var : variables)
		    {
		    	//we only take unassigned vars
		    	if ( var->isAssigned() )
		    		continue;

		    	//initialize var count = 0
		    	countMap.insert(std::pair<Variable*, int>(var, 0));

		    	//get all the neighbours
			    ConstraintNetwork::VariableSet neighbors = solver.getNetwork().getNeighborsOfVariable( var );

				//for each neighbour, count occurence of the values v has, the more counts, the more constrained
				for ( Variable* neigh : neighbors ) 
				    if ( !neigh->isAssigned() )    
				    	countMap[var] += 1;
		    }  
		    cout << "test3" << endl;

		    // it means all assigned, or maybe some errors
		    if ( countMap.size() == 0)
		    	printf("Finished!!\n");
			//sort the map

			cout << "test4" << endl;
			//reverse key-value in a vector, so as to sort by map's value, which is vector's key
		    vector<std::pair<int, Variable*>> reverseMap;
		    for (std::map<Variable*, int>::iterator it=countMap.begin(); it!=countMap.end(); ++it)
		    {
		    	std::pair<int, Variable*> tmp;
		    	tmp.first = it->second;
		    	tmp.second = it->first;
		    	reverseMap.push_back(tmp);
		    }
		    
		    cout << "test5" << endl;
		    //sort
			std::sort(reverseMap.begin(), reverseMap.end());

			cout << "test6" << endl;
			Variable* front = reverseMap.front().second;

			Variable* back = reverseMap.back().second;
			printf("%d %d", front->size(), back->size());
			printf( "%s: %d %s:%d is the current sort.\n", 
				(std::string) front->getName(), reverseMap.front().first, 
				(std::string) back->getName(), reverseMap.back().first );
		*///==================================
	solver.solve();

	if ( solver.haveSolution() )
	{
		cout << solver.getSolution().toString() << endl;
		cout << "Assignments: " << trail.getPushCount() << endl;
		cout << "Backtracks: "  << trail.getUndoCount() << endl;
	}
	else
	{
		cout << "Failed to find a solution" << endl;
	}

	return 0;
}
