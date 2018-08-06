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
  size_t find_first, find_second;
  string close_tag, begin_tag;

  close_tag = "</" + first;
  begin_tag = "<" + second;

  for (unsigned int i=0; i < src_code.size(); i++)
  {
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
   
  if (idx_second < idx_first)
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

//=============================== HEADER VALID =================================
bool header(vector<string>src_code, string tag_first_query)
{
	bool result = true;
	unsigned int flag_query, flag_close;
	string tag_src;

	size_t tg_first_src;
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
			}

			begin_tag_query = src_code[i].find(tag_first_query);
			if (begin_tag_query != std::string::npos)
			{
				flag_query = i;

				break;
			}				
		}
		if (flag_query < flag_close)
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
#endif