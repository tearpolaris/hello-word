#ifndef __QUERY_H__
#define __QUERY_H__

#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
#define FIND '~'
#define DOT '.'
#define SPACE ' '
using namespace std;

//=========================== COMPARE TWO TAG =============================
bool compare_tag(vector<string>src_code, string first, string second)
{
  int idx_first, idx_second;
  idx_first = idx_second = -1;
  size_t find_first, find_second, find_tag_first;
  string close_tag, begin_tag;

  close_tag = "</" + first;
  begin_tag = "<" + second;

  for (unsigned int i=0; i < src_code.size(); i++)
  {
	  find_tag_first 
	  find_first = src_code[i].find(close_tag);
	  if (find_first != std::string::npos)
	  {
		  idx_first = i;
	  }
	  find_second = src_code[i].find(begin_tag);
	  if (find_second != std::string::npos)
	  {
		  idx_second = i;
		  break;
	  }
  }
   
  if ((idx_second < idx_first) || (idx_first == -1))
  {
	  cout << "Tag second is within tag first" << endl;
	  return true;
  }
  else
  {
	  cout << "Tag second is out of range tag first" << endl;
	  return false;
  }
}

//========================== VALID OF HEADER ============================
//Input    : 1. source code                              //
//           2. first tag query                             //
//                                                                      //
//Type     : 1.string                                                   //  
//           2. size_t                                                  //
//                                                                      // 
//Function : get atribute of query                                      //
//======================================================================//
bool header(vector<string>src_code, string tag_first_query)
{
	bool result = true;
	int flag_query, flag_close;
	string tag_src;
	size_t tg_first_src;

	flag_query = flag_close = -1;
	tag_first_query = "<" + tag_first_query;
	tg_first_src = src_code[0].find(tag_first_query);//find tag query in source code

	if (tg_first_src == std::string::npos)
	{
		size_t temp_space; //space in first line of source code
		temp_space = src_code[0].find(SPACE);//find the end of tag

		tag_src = src_code[0].substr(1,temp_space -1);//create string first tag and ignore <
		string close_tag = "</" + tag_src;//create close first tag of source code
		cout << "Tag source is: " << tag_src << " && " << "Tag close is: " << close_tag << endl;

		for (unsigned int i=1; i <src_code.size(); i++)
		{
			size_t pos_close_tag;
			size_t begin_tag_query;
			pos_close_tag = src_code[i].find(close_tag);
			if (pos_close_tag != std::string::npos)
			{
				flag_close = i;
				size_t tmp_space;
				tmp_space = src_code[i+1].find(SPACE);
				tag_src = src_code[i+1].substr(1, temp_space-1);
				close_tag = "</" + tag_src;
			} 

			begin_tag_query = src_code[i].find(tag_first_query);
			if (begin_tag_query != std::string::npos)
			{
				flag_query = i;
				break;
			}				
		}
		cout << "flag_query: " << flag_query << "  " << "flag_close: " << flag_close << endl; 
		if ((flag_query > flag_close) && (flag_close != -1))
		{
			cout << "Tag is valid!" << endl;
			result = true;
		}
		else
		{
			cout << "Tag is not valid!" << endl;
			result = false;
		}
	}
	return result;
}

vector<string> sequence_query (string a)
{
	vector<string> tmp_str;
	unsigned int pos = 0;
	bool exit = false;

	for (unsigned int i=0; (i < a.length()) && (exit == false); i++)
	{
		if (a[i] == FIND)
		{
			tmp_str.push_back(a.substr(pos,i-pos));
			exit = true;
		}
		if (a[i] == DOT)
		{
			tmp_str.push_back(a.substr(pos, i -pos));
			pos = i+1;
		}
	}
	return tmp_str;
}

void printf_vector(vector<string> a)
{
	for (unsigned int i = 0; i < a.size(); i++)
	{
		cout << a[i] << endl;
	}
}

bool result_query(vector<string> src_code, vector<string> query)
{
	bool bHeader = true;
 	for (unsigned int i=1; (i < query.size()) && (bHeader == true); i++)
	{
		//bool bCompare = false;
		bHeader = compare_tag(src_code,query[i], query[i+1]);
	}
	return bHeader;
}

//======================== GET STRING OF ATTRIBUTE ========================
//Input    : 1. line code need to get value                             //
//           2. pos of tag name respectively                            //
//                                                                      //
//Type     : 1.string                                                   //  
//           2. size_t                                                  //
//                                                                      // 
//Function : get atribute of query                                      //
//======================================================================//
string get_attribute (string query)
{
	size_t pos = 0;
	string attribute;

	pos = query.find(FIND);
	attribute = query.substr(pos+1, query.length()- pos - 1); 
	return attribute;
}

//======================== GET VALUE OF ATTRIBUTE ========================
//Input    : 1. line code need to get value                             //
//           2. pos of tag name respectively                            //
//                                                                      //
//Output   : string of value                                                                      //
//Type     : 1.string                                                   //  
//           2. size_t                                                  //
//                                                                      // 
//Function : get value of atribute                                      //
//======================================================================//
string get_value (string line_code, size_t pos)
{
	string tmp_value = "";
	size_t tmp_pos_first, tmp_pos_second;

	tmp_pos_first = line_code.find(" \" ", pos);
	if (tmp_pos_first != std::string::npos)
	{
		tmp_pos_second = line_code.find(" \" ", tmp_pos_first + 1);
		if (tmp_pos_second != std::string::npos)
		{
			tmp_value = line_code.substr(tmp_pos_first+1, tmp_pos_second - tmp_pos_first-1);
		}		
	}
	return tmp_value;
}
#endif