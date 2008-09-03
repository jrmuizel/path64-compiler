#include <map>
#include <string>

#include "wintrinsic.h"
#include "errors.h"

static std::map <std::string, int> builtin_name_to_intrinsic;


INTRINSIC EXTENSION_INTRINSIC_From_Name(const char * name)
{
  INTRINSIC i =  builtin_name_to_intrinsic[name];
  if (i<=0) {
    DevWarn("%s has no id %d\n", name, i);
  }
  return i;
}


void EXTENSION_add_INTRINSIC_to_Map(const char* c, INTRINSIC i)
{
  builtin_name_to_intrinsic[c] = i;
}
