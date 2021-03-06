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
 * Note: remember to trail.push variables before you change their domain
 * Return: true is assignment is consistent, false otherwise
 */
bool BTSolver::forwardChecking ( void )
{
    for ( Constraint* c : network.getModifiedConstraints())
    {
        for( Variable* v : c->vars)
        {
			if ( v->isAssigned() ) {
				for (Variable *neighbor : c->vars)
				{
					int value = v->getDomain().getValues()[0];
					if (v != neighbor && neighbor->getDomain().contains(value))
					{
                        if (neighbor->getDomain().size() == 1)
                        {
                            return false;
                        }
						trail->push(neighbor);
						neighbor->removeValueFromDomain(value);
					}
				}
			}
        }
		if( !c->isConsistent() ){
			return false;
		}
    }
    return true;
}



/** Used to help with norvig check */
bool BTSolver::eliminateNeighbors( Variable* v ){

    //std::cout << v->toString() << std::endl;
    std::vector< Variable* > neighbors = network.getNeighborsOfVariable( v );
    int value = v->getDomain().getValues()[0];
    for ( Variable* neighbor : neighbors )
    {
        if ( v != neighbor && neighbor->getDomain().contains( value ) )
        {
            if ( neighbor->isAssigned() )
            {
                return false;
            }
            trail->push( neighbor );
            neighbor->removeValueFromDomain( value );
            if ( neighbor->getDomain().size() == 1 )
            {
                bool val = eliminateNeighbors( neighbor );
                if ( !val )
                    return false;
                for ( Constraint* c : network.getConstraintsContainingVariable( neighbor )){
                    if ( !c->isConsistent() )
                        return false;
                }
            }
        }
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
 * Note: remember to trail.push variables before you change their domain
 * Return: true is assignment is consistent, false otherwise
 */
bool BTSolver::norvigCheck ( void )
{
    std::vector< Constraint* > modifiedConstraints = network.getModifiedConstraints();
    for( Constraint* c : modifiedConstraints )
    {
        for( Variable* v : c->vars )
        {
            if ( v->isAssigned() )
            {
                bool val = eliminateNeighbors( v );
                if( !val )
                    return false;
            }
        }
        if( !c->isConsistent() ){
            return false;
        }
    }
	return true;
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
    Variable* min_v = nullptr;
    int min_value = INT_MAX;
    for ( Variable* v : network.getVariables())
    {
        if ( !(v->isAssigned()) )
        {
            int domainSize = v->getDomain().size();
            if ( domainSize < min_value)
            {
                min_value = domainSize;
                min_v = v;
            }
        }
    }

    return min_v;
}

/**
 * Part 2 TODO: Implement the Degree Heuristic
 *
 * Return: The unassigned variable with the most unassigned neighbors
 */
Variable* BTSolver::getDegree ( void )
{
    Variable* max_v = nullptr;
    int max_value = INT_MIN;
    for ( Variable* v : network.getVariables())
    {
        if ( !(v->isAssigned()) )
        {
            int unassignedNeighbors = 0;
            for( Variable* neighbor : network.getNeighborsOfVariable( v ) )
            {
                if( !neighbor->isAssigned() )
                    unassignedNeighbors++;
            }
            if( max_value < unassignedNeighbors ){
                max_value = unassignedNeighbors;
                max_v = v;
            }
        }
    }
    return max_v;
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
    std::vector<Variable*> MRV_array;

    int minValue = INT_MAX;
    for ( Variable* v : network.getVariables() )
    {
        if ( !v->isAssigned() )
        {
            int domainSize = v->getDomain().size();
            if ( domainSize < minValue)
            {
                minValue = domainSize;
                MRV_array.clear();
                MRV_array.push_back( v );
            }
            else if ( domainSize == minValue)
            {
                MRV_array.push_back( v );
            }
        }
    }

    if (MRV_array.size() == 1) {
        return MRV_array[0];
    }

    int maxNeighbors = INT_MIN;
    Variable* maxV = nullptr;
    int unassignedNeighbors = 0;
    for( Variable* v : MRV_array )
    {
        unassignedNeighbors = 0;
        for ( Variable* neighbor : network.getNeighborsOfVariable(v) )
        {
            if( !neighbor->isAssigned() )
                unassignedNeighbors++;
        }
        if( maxNeighbors < unassignedNeighbors ){
            maxNeighbors = unassignedNeighbors;
            maxV = v;
        }
    }

    return maxV;
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
// Value Selectors
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
 * The Least constraining value is the one that will knock the least
 * values out of it's neighbors domain.
 *
 * Return: A list of v's domain sorted by the LCV heuristic
 *         The LCV is first and the MCV is last
 */
vector<int> BTSolver::getValuesLCVOrder ( Variable* v )
{
    std::vector<int> result;
    std::vector<int> varDomain = v->getDomain().getValues();
    unsigned int long domainSize = v->getDomain().size();
    std::vector< std::pair<int, int> >countVector;

    for (int d : varDomain ){
		std::pair<int, int> current(0, d);
        for ( Variable* neighbor : network.getNeighborsOfVariable( v ) ){
            std::vector<int> vd = neighbor->getDomain().getValues();
            if( std::find(vd.begin(), vd.end(), d) != vd.end() )
			{
                current.first += 1;
            }
        }
		countVector.push_back(current);
    }

	std::sort(countVector.begin(), countVector.end());

	for(auto p : countVector)
	{
		result.push_back(p.second);
	}


    return result;
}

/**
 * Optional TODO: Implement your own advanced Value Heuristic
 *
 * Completing the three tourn heuristic will automatically enter
 * your program into a tournament.
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
