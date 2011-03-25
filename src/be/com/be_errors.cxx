/*
  Copyright (C) 2011 PathScale Inc. All Rights Reserved.
*/

#include "be_errors.h"
#include <cstdarg>
#include "ir_reader.h"

void ErrMsgSrcpos ( INT ecode, SRCPOS srcpos, ... )
{
  INT32  line = Srcpos_To_Line(srcpos);
  const char   *fname = NULL;
  const char   *dname;

  va_list vp;
  va_start ( vp, srcpos );

  IR_Srcpos_Filename(srcpos, &fname, &dname);
  ErrMsg_Report ( ecode, line, fname, vp );
  va_end ( vp );
}

