@echo off
set DIR=%CD:\=\\%
echo #include "version.h"
echo const char *const cset_id = "";
echo const char *const build_root = "%DIR%";
echo const char *const build_host = "";
echo const char *const build_user = "%USERNAME%";
echo const char *const build_date = "%DATE%";
