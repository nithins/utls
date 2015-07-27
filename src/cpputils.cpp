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
#include <boost/thread.hpp>

using namespace std;

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

std::string __format_ffl(const char *file,const char* func,int line)
{
  return (std::string("(")+basename(file)+","+func+","+utls::to_string(line)+")");
}

#include <sstream>
#include <fstream>

#include <vector>

#include <tr1/functional>


/*===========================================================================*/

using namespace std;

namespace utls {
/*---------------------------------------------------------------------------*/

void trim(std::string &s)
{
  s.erase(s.begin(), std::find_if
          (s.begin(), s.end(),
           std::not1(std::ptr_fun<int, int>(std::isspace))));
  s.erase(std::find_if
          (s.rbegin(), s.rend(),
           std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
}

/*---------------------------------------------------------------------------*/

file_line_iterator::file_line_iterator(const char * f,char c_char):
  is(new std::ifstream(f)),c_char(c_char)
{
  ENSUREV(is->is_open(),"cannot read the file",f);
  increment();
}

/*---------------------------------------------------------------------------*/

void file_line_iterator::increment()
{
  value.clear();

  while(is && value.size() == 0)
  {
    if(is->eof())
    {
      is.reset();
      value.clear();
      break;
    }

    std::getline(*is,value);

    int p = value.find(c_char);

    if ( p < value.size() )
      value = value.substr(0,p);

    trim(value);
  }
}

/*---------------------------------------------------------------------------*/

bool file_line_iterator::equal(file_line_iterator const& other) const
{
  if(!is && !other.is)
    return true;

  if(!is || !other.is)
    return false;

  ENSURE(is == other.is, "cannot compare distinct istream iters");

  return is->tellg() == other.is->tellg();
}

/*---------------------------------------------------------------------------*/

const std::string & file_line_iterator::dereference() const
{
  ENSURE(is,"dereferencing past end of line stream");
  return value;
}

/*---------------------------------------------------------------------------*/

namespace detail{
std::string __classFunction__(const std::string& prettyFunction)
{
  std::string str(prettyFunction);

  str = str.substr(str.find(" ")+1);
  str = str.substr(0,str.find("("));

  size_t first_colon  =  str.rfind("::");

  if(first_colon != std::string::npos)
  {
    size_t second_colon = str.substr(0,first_colon).rfind("::");

    if(second_colon != std::string::npos)
      str   = str.substr(second_colon+2);
  }
  return str;
}

/*---------------------------------------------------------------------------*/

std::string __trace_indenter_t__::get_indent()
{
  std::string s;

  for(int i = 0 ; i < s_indent; ++i)
    s += "  ";

  return s;
}


}

/*---------------------------------------------------------------------------*/

boost::mutex logger::s_mutex;
logger       logger::s_logger;

int    detail::__trace_indenter_t__::s_indent = 0;

/*---------------------------------------------------------------------------*/

} // namespace utl

/*===========================================================================*/



