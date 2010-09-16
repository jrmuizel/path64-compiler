/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

   Copyright (C) STMicroelectronics All Rights Reserved.

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   Path64 is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with Path64; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.

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
