/***************************************************************************
 *   Copyright (C) 2009 by Nithin Shivashankar,   *
 *   nithin@gauss   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <cpputils.h>

using namespace std;


genericException::genericException ( string str ) : m_str ( str )
{
}

string genericException::str() const
{
  return m_str;
}

ostream &operator<< ( ostream &stream, const genericException &e )
{
  stream << e.str();

  return stream;
}

string stripLineComments ( const string& line, const char& comment_char )
{
  size_t pos = line.find ( comment_char );

  if ( pos == string::npos )
    return line;
  else
    return string ( line.begin(), line.begin() + pos );
}

string stripLeadingWS ( const string& line )
{
  size_t npos = 0;

  while ( ( line.begin() + npos != line.end() ) &&
          (
            ( * ( line.begin() + npos ) == ' ' ) ||
            ( * ( line.begin() + npos ) == '\n' ) ||
            ( * ( line.begin() + npos ) == '\t' )
          )
        )
    npos++;

  return string ( line.begin() + npos, line.end() );
}

string stripTrailingWS ( const string& line )
{
  size_t npos = 0;

  while ( ( line.begin() != line.end() - npos ) &&
          (
            ( * ( line.end() - npos - 1 ) == ' ' ) ||
            ( * ( line.end() - npos - 1 ) == '\n' ) ||
            ( * ( line.end() - npos - 1 ) == '\t' )
          )
        )
    npos++;

  return string ( line.begin(), line.end() - npos );
}

string stripWS ( const string &line )
{
  return stripLeadingWS ( stripTrailingWS ( line ) );
}

void tokenize_string ( const string& str,
                       vector<string>& tokens,
                       const string& delimiters  )
{
  // Skip delimiters at beginning.
  string::size_type lastPos = str.find_first_not_of ( delimiters, 0 );
  // Find first "non-delimiter".
  string::size_type pos     = str.find_first_of ( delimiters, lastPos );

  while ( string::npos != pos || string::npos != lastPos )
  {
    // Found a token, add it to the vector.
    tokens.push_back ( str.substr ( lastPos, pos - lastPos ) );
    // Skip delimiters.  Note the "not_of"
    lastPos = str.find_first_not_of ( delimiters, pos );
    // Find next "non-delimiter"
    pos = str.find_first_of ( delimiters, lastPos );
  }
}

void split_string ( const string & line_str, vector<string> & tokens, const string & split_str )
{
  size_t npos_start = 0;
  size_t npos_end   = line_str.find ( split_str );

  while ( npos_end != string::npos )
  {
    if ( npos_start != npos_end )
      tokens.push_back ( string ( line_str.begin() + npos_start, line_str.begin() + npos_end ) );

    npos_start = npos_end + split_str.size();

    npos_end   = line_str.find ( split_str, npos_start );
  }

  npos_end = line_str.size();

  if ( npos_start != npos_end )
    tokens.push_back ( string ( line_str.begin() + npos_start, line_str.begin() + npos_end ) );

}

