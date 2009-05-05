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
