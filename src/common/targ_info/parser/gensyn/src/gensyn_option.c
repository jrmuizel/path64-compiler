
#include "gensyn_option.h"

struct gensyn_option gensyn_option = {
  { UTILS_OPTION_OPEN,   UTILS_VERSION_NAME, "Base options", NULL, UTILS_OPTION_LINE },

  { UTILS_OPTION_TAG,    "help",    NULL,               NULL, 0,                   "display this message" },
  { UTILS_OPTION_TAG,    "version", NULL,               NULL, UTILS_OPTION_CASE,   "display version" },
  { UTILS_OPTION_TAG,    "verbose", NULL,               NULL, UTILS_OPTION_HIDDEN, "be verbose" },

  { UTILS_OPTION_SCALAR, "lc",      "file",             NULL, UTILS_OPTION_ONCE,   "output linker C file" },
  { UTILS_OPTION_SCALAR, "dc",      "file",             NULL, UTILS_OPTION_ONCE,   "output disassembler C file" },
  { UTILS_OPTION_SCALAR, "ac",      "file",             NULL, UTILS_OPTION_ONCE,   "output assembler C file (default=stdout)" },
  { UTILS_OPTION_SCALAR, "ah",      "file",             NULL, UTILS_OPTION_ONCE,   "output assembler C header file" },
  { UTILS_OPTION_SCALAR, "rctype",  "maxcore|corxpert", NULL, UTILS_OPTION_ONCE,   "Type of reconfiguration" },

  // Set IGNORE when __TOOLKIT__ defined
  { UTILS_OPTION_SCALAR, "type", "extension|core",      NULL, UTILS_OPTION_ONCE, "generates a parser dedicated to an extension or to the core" },
  { UTILS_OPTION_SCALAR, "sname", "word",               NULL, UTILS_OPTION_ONCE, "define extension short name" },
  { UTILS_OPTION_SCALAR, "lname", "string",             NULL, UTILS_OPTION_ONCE, "define extension long name" },

  { UTILS_OPTION_CLOSE }
};

