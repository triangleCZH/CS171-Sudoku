#include"Constraint.hpp"

Constraint::Constraint ( void )
{

}

Constraint::Constraint ( VariableSet vars )
{
	this->vars = vars;
}

void Constraint::addVariable ( Variable* v )
{
	vars.push_back( v );
}

int Constraint::size ( void )
{
	return vars.size();
}

bool Constraint::contains ( Variable* v )
{
	if ( std::find( vars.begin(), vars.end(), v ) != vars.end() )
		return true;
	return false;
}

bool Constraint::isModified ( void )
{
	for ( Variable* var:vars )
	{
		if ( var->isModified() )
		{
			return true;
		}
	}
	return false;
}

int Constraint::getConflicts ( void )
{
	int numConflicts = 0;
	for ( Variable* var:vars )
	{
		for ( Variable* otherVar : vars )
		{
			if ( *var == *otherVar )
			{
				continue;
			}
			else if ( var->getAssignment() == otherVar->getAssignment() )
			{
				numConflicts++;
			}
		}
	}
	return numConflicts;
}

bool Constraint::propagateConstraint ( void )
{
	for ( Variable* var:vars )
	{
		if ( !var->isAssigned() )
			continue;

		int varAssignment = var->getAssignment();
		for ( Variable* otherVar:vars )
		{
			if ( var == otherVar )
				continue;

			if ( otherVar->size() == 1 && otherVar->getAssignment() == varAssignment )
				return false;

			if ( otherVar->size() == 1 )
				otherVar->removeValueFromDomain( varAssignment );
		}
	}
	return true;
}

bool Constraint::isConsistent ( void )
{
	return propagateConstraint();
}

bool Constraint::operator== ( const Constraint &other ) const
{
    if ( vars.size() == other.vars.size() )
    {
        for ( Variable* v : vars )
        {
            if ( std::find( other.vars.begin(), other.vars.end(),v ) == other.vars.end() )
                return false;
        }
        return true;
    }
    return false;
}

std::string Constraint::toString ( void )
{
	std::stringstream ss;
	ss << "{";
	std::string delim = "";
	for ( Variable* v:vars )
	{
		ss<<delim<<v->getName();
		delim = ",";
	}
	ss<<"}";
	return ss.str();
}
