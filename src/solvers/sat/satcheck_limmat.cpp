/*******************************************************************\

Module:

Author: Daniel Kroening, kroening@kroening.com

\*******************************************************************/


#include <cassert>


#include "satcheck_limmat.h"

extern "C"
{
#include "limmat.h"
}

satcheck_limmatt::satcheck_limmatt()
{
  solver=new_Limmat(NULL);
}

satcheck_limmatt::~satcheck_limmatt()
{
  if(solver!=NULL)
    delete_Limmat(solver);
}

tvt satcheck_limmatt::l_get(literalt a) const
{
  if(a.is_true())
    return tvt(true);
  else if(a.is_false())
    return tvt(false);

  tvt result;
  unsigned v=a.var_no();

  assert(v<assignment.size());

  switch(assignment[v])
  {
    case 0: result=tvt(false); break;
    case 1: result=tvt(true); break;
    default: result=tvt(tvt::tv_enumt::TV_UNKNOWN); break;
  }

  if(a.sign())
    result=!result;

  return result;
}

const std::string satcheck_limmatt::solver_text()
{
  return std::string("Limmat version ")+version_Limmat();
}

void satcheck_limmatt::copy_cnf()
{
  for(clausest::iterator it=clauses.begin();
      it!=clauses.end();
      it++)
      // it=clauses.erase(it))
  {
    int *clause=new int[it->size()+1];

    for(unsigned j=0; j<it->size(); j++)
      clause[j]=(*it)[j].dimacs();

    // zero-terminated
    clause[it->size()]=0;

    add_Limmat(solver, clause);

    delete clause;
  }
}

propt::resultt satcheck_limmatt::prop_solve()
{
  copy_cnf();

  {
    std::string msg=
      std::to_string(maxvar_Limmat(solver))+" variables, "+
      std::to_string(clauses_Limmat(solver))+" clauses";
    messaget::status() << msg << messaget::eom;
  }

  int status=sat_Limmat(solver, -1);

  {
    std::string msg;

    switch(status)
    {
     case 0:
      msg="SAT checker: instance is UNSATISFIABLE";
      break;

     case 1:
      msg="SAT checker: instance is SATISFIABLE";
      break;

     default:
      msg="SAT checker failed: unknown result";
      break;
    }

    messaget::status() << msg << messaget::eom;
  }

  if(status==0)
  {
    assignment.clear();
    return P_UNSATISFIABLE;
  }

  if(status==1)
  {
    assignment.resize(no_variables()+1, 2); // unknown is default

    for(const int *a=assignment_Limmat(solver); *a!=0; a++)
    {
      int v=*a;
      if(v<0)
        v=-v;
      assert((unsigned)v<assignment.size());
      assignment[v]=(*a)>=0;
    }

    return P_SATISFIABLE;
  }

  return P_ERROR;
}
