/*******************************************************************\

Module:

Author: Daniel Kroening, kroening@kroening.com

\*******************************************************************/


#include <ostream>

#include "ansi_c_scope.h"

void ansi_c_scopet::print(std::ostream &out) const
{
  out << "Prefix: " << prefix << "\n";

  for(const auto &name : name_map)
  {
    out << "  ID: " << name.first
        << " CLASS: " << static_cast<int>(name.second.id_class)
        << "\n";
  }
}
