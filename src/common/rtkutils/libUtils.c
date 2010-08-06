
#include "libUtils.h"

int libUtilsInit( ) {

	static int first = FALSE;
	if ( ! first ) {

		utils_option_open                .next = & utils_option_print_select_info;
		utils_option_print_select_info   .next = & utils_option_print_select_warning;
		utils_option_print_select_warning.next = & utils_option_print_select_error;
		#ifdef _UTILS_DEBUG_PRINT
		utils_option_print_select_error  .next = & utils_option_print_select_debug;
		utils_option_print_select_debug  .next = & utils_option_close;
		#else // removes "utils.debug" option when not in debug mode
		utils_option_print_select_error  .next = & utils_option_close;
		#endif
		utils_option_close               .next = NULL;

		if ( ! utilsOptionInit( & utils_option_open, 0 ) ) {
			return FALSE;
		}

		if ( ! utilsVersionInit( & libUtils_version ) ) {
			return FALSE;
		}

		first = TRUE;
	}

	return TRUE;
}

int libUtilsTerminate( ) {

	return TRUE;
}

