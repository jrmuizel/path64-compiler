
#ifndef gensyn_option_h
#define gensyn_option_h

#include "libUtils.h"

#include "gensyn_version.h"

extern struct gensyn_option {
	utilsOption open;

	utilsOption help;
	utilsOption version;
	utilsOption verbose;

	utilsOption lcfile;
	utilsOption dcfile;
	utilsOption acfile;
	utilsOption ahfile;
	utilsOption rctype;

	// Set IGNORE when __TOOLKIT__ defined
	utilsOption outtype;
	utilsOption short_name;
	utilsOption long_name;

	utilsOption close;
} gensyn_option;

#endif

