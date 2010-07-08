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
////    Declaration of XmlUtils functions.                                       ////
////                                                                             ////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


/**
 * \file XmlUtils.h
 */


#ifndef XmlUtils_h
#define XmlUtils_h

#include <iostream>
#include <string>



namespace DfgForIse {

  ///////////////////////////////////////////////////////////////////////////////////
  ////    Declaration of transformation from and to xml special chars.           ////
  ///////////////////////////////////////////////////////////////////////////////////

  /** Transform a text by replacing special characters with their representation
   *  in XML.
   * @param text A string in which special characters have to be transformed.
   * @return The modified string with special characters replaced.
   */
  std::string text2xml(std::string text);


  /** Transform an xml text by replacing representation of special characters in XML.
   * @param xmlText A string representing a text in xml format.
   * @return The modified string with special characters replaced.
   */
  std::string xml2text(std::string xmlText);



};



#endif // XmlUtils_h
