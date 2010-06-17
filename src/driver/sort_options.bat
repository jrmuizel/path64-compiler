@echo off
rem 
rem Copyright (C) 2006. PathScale Inc. All Rights Reserved.
rem 
rem 
rem 
rem Path64 is free software; you can redistribute it and/or modify it
rem under the terms of the GNU General Public License as published by
rem the Free Software Foundation version 3
rem
rem Path64 is distributed in the hope that it will be useful, but WITHOUT
rem ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
rem or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
rem License for more details.
rem
rem You should have received a copy of the GNU General Public License
rem along with Path64; see the file COPYING.  If not, write to the Free
rem Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
rem 02110-1301, USA.
rem
rem
rem
rem %1 == sort_options.exe full path
rem
SET sort_options_exe=%1
%sort_options_exe% tmp.options.cpp
echo %%%%%% OPTIONS
type tmp.options
echo %%%%%% COMBINATIONS
type tmp.options.combo
del /Q tmp.options*

