/* 

  Copyright (C) 2008 ST Microelectronics, Inc.  All Rights Reserved. 

  This program is free software; you can redistribute it and/or modify it 
  under the terms of version 2 of the GNU General Public License as 
  published by the Free Software Foundation. 
  This program is distributed in the hope that it would be useful, but 
  WITHOUT ANY WARRANTY; without even the implied warranty of 
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 

  Further, this software is distributed without any warranty that it is 
  free of the rightful claim of any third person regarding infringement 
  or the like.  Any license provided herein, whether implied or 
  otherwise, applies only to this software file.  Patent licenses, if 
  any, provided herein do not apply to combinations of this program with 
  other software, or any other product whatsoever. 
  You should have received a copy of the GNU General Public License along 
  with this program; if not, write the Free Software Foundation, Inc., 59 
  Temple Place - Suite 330, Boston MA 02111-1307, USA. 

  Contact information:  ST Microelectronics, Inc., 
  , or: 

  http://www.st.com 

  For further information regarding this notice, see: 

  http: 
*/
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
////                                                                             ////
////    Definition of XmlUtils functions.                                        ////
////                                                                             ////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


/**
 * \file XmlUtils.cxx
 */


#include "XmlUtils.h"

#include "GraphmlTypes.h"

#include <iostream>
#include <string>



namespace DfgForIse {



  ///////////////////////////////////////////////////////////////////////////////////
  ////    Declaration of transformation from and to xml special chars.           ////
  ///////////////////////////////////////////////////////////////////////////////////

  std::string text2xml(std::string text) {
    // To be sure to not transform still transformed special characters.
    // Ex. : &lt; must not be transformed into &amp;lt;
    std::string result = xml2text(text);
    size_t found;
    found = result.find("&");
    while (found != std::string::npos) {
      result.replace(found, 1, "&amp;");
      found = result.find("&", found + 5);
    }
    found = result.find("<");
    while (found != std::string::npos) {
      result.replace(found, 1, "&lt;");
      found = result.find("<", found + 4);
    }
    found = result.find(">");
    while (found != std::string::npos) {
      result.replace(found, 1, "&gt;");
      found = result.find(">", found + 4);
    }
    found = result.find("\"");
    while (found != std::string::npos) {
      result.replace(found, 1, "&quot;");
      found = result.find("\"", found + 6);
    }
    found = result.find("'");
    while (found != std::string::npos) {
      result.replace(found, 1, "&#39;");
      found = result.find("'", found + 5);
    }
    return result;
  };


  std::string xml2text(std::string xmlText) {
    std::string result = xmlText;
    size_t found;
    found = result.find("&amp;");
    while (found != std::string::npos) {
      result.replace(found, 5, "&");
      found = result.find("&amp;", found + 1);
    }
    found = result.find("&lt;");
    while (found != std::string::npos) {
      result.replace(found, 4, "<");
      found = result.find("&lt;", found + 1);
    }
    found = result.find("&gt;");
    while (found != std::string::npos) {
      result.replace(found, 4, ">");
      found = result.find("&gt;", found + 1);
    }
    found = result.find("&quot;");
    while (found != std::string::npos) {
      result.replace(found, 6, "\"");
      found = result.find("&quot;", found + 1);
    }
    found = result.find("&#39;");
    while (found != std::string::npos) {
      result.replace(found, 5, "'");
      found = result.find("&#39;", found + 1);
    }
    return result;
  };



};
