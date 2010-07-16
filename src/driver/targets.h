/*
  Copyright (C) 2010 PathScale Inc. All Rights Reserved.
*/


#ifndef __DRIVER_TARGETS_H__
#define __DRIVER_TARGETS_H__


// Returns path to target library path
// Returned memory should be freed by caller
char * target_library_path();


// Returns path to target runtime path (location of crtX.o)
// Returned memory should be freed by caller
char * target_runtime_path();


#endif // __DRIVER_TARGETS_H__

