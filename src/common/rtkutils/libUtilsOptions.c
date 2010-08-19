
#include "libUtilsOptions.h"

utilsOption utils_option_open = {
	UTILS_OPTION_OPEN, UTILS_VERSION_NAME, "Default options from libUtils", NULL, UTILS_OPTION_LINE
};

utilsOption utils_option_print_select_info = {
	UTILS_OPTION_LIST, "utils.info", "selection",
	NULL, 0,
	"Set filter selection of informations"
};

utilsOption utils_option_print_select_warning = {
	UTILS_OPTION_LIST, "utils.warning", "selection",
	NULL, 0,
	"Set filter selection of warnings"
};

utilsOption utils_option_print_select_error= {
	UTILS_OPTION_LIST, "utils.error", "selection",
	NULL, 0,
	"Set filter selection of errors"
};

utilsOption utils_option_print_select_debug= {
	UTILS_OPTION_LIST, "utils.debug", "selection",
	NULL, UTILS_OPTION_HIDDEN,
	"Set filter selection of debug messages"
};

utilsOption utils_option_close = {
	UTILS_OPTION_CLOSE
};


