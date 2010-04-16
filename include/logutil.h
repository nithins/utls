/***************************************************************************
 *   Copyright (C) 2009 by Nithin Shivashankar,                            *
 *   nithin@gauss                                                          *
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
#ifndef __LOG_UTIL_H_INCLUDED__
#define __LOG_UTIL_H_INCLUDED__

#include <iostream>
#include <sstream>
#include <fstream>


std::string get_cur_time();

#define _LOG_TO_FILE(_LOG_STR,_LOG_FILE)\
{\
  std::fstream __FS;\
  __FS.open(_LOG_FILE,std::ios::ate|std::ios::out);\
  __FS<<_LOG_STR;\
}\

#define _LOG(logstr)\
std::cout<<get_cur_time() <<"::LOG      ::" << logstr <<std::endl

#define _ERROR(logstr)\
std::cerr <<"ERROR    ::"<<__FILE__<<"  :"<<__LINE__<<"  :" <<__func__<<":"<< logstr <<std::endl

#define _WARNING(logstr)\
std::cerr <<"WARNING  ::" << logstr <<std::endl

#define _LOG_V(logstr)\
//std::cout <<"LOG V    ::" << logstr <<std::endl

#define _LOG_VAR(var)\
std::cout<<"LOG      ::"<<#var<<" = "<<var<<std::endl;

#define _LOG_FILE_N_FUNC() _LOG(__FILE__<<"::"<<__func__);

#define _SSTR(EXP)\
((std::stringstream&)((std::stringstream())<<EXP)).str()



template <typename iter_t>
inline void log_range ( iter_t begin,iter_t end,const char * title = NULL,bool with_ind = false )
{

  if ( title != NULL )
  {
    std::string title_str ( title );
    std::string ln1_str ( title_str.size(),'=' );
    std::string ln2_str ( title_str.size(),'-' );

    _LOG ( ln1_str );
    _LOG ( title_str );
    _LOG ( ln2_str );
  }


  std::stringstream ss;

  unsigned int i = 0;

  for ( iter_t pos = begin;pos !=end; ++pos )
  {
    std::stringstream ss_temp;

    if ( with_ind )
    {
      ss_temp<<i++<<":";
    }

    ss_temp << *pos<<" ";

    ss<<ss_temp.str();
  }
  _LOG ( ss.str() );

  if ( title != NULL )
  {
    std::string title_str ( title );
    std::string ln1_str ( title_str.size(),'=' );
    _LOG ( ln1_str );
  }
}

template <typename iter_t,typename get_val_ftor_t>
inline void log_range ( iter_t begin,iter_t end,get_val_ftor_t get_val_ftor,const char * title = NULL,bool with_ind = false )
{

  if ( title != NULL )
  {
    std::string title_str ( title );
    std::string ln1_str ( title_str.size(),'=' );
    std::string ln2_str ( title_str.size(),'-' );

    _LOG ( ln1_str );
    _LOG ( title_str );
    _LOG ( ln2_str );
  }


  std::stringstream ss;

  unsigned int i = 0;

  for ( iter_t pos = begin;pos !=end; ++pos )
  {
    std::stringstream ss_temp;

    if ( with_ind )
    {
      ss_temp<<i++<<":";
    }

    ss_temp << get_val_ftor(*pos)<<" ";

    ss<<ss_temp.str();
  }
  _LOG ( ss.str() );

  if ( title != NULL )
  {
    std::string title_str ( title );
    std::string ln1_str ( title_str.size(),'=' );
    _LOG ( ln1_str );
  }
}

#endif
