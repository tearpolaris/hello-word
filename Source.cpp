//#include <sstream>
#include "query.h"

int main() 
{
while(1)
{
	system("cls");

    vector<string> src_code;
	vector<string> query;
	vector<string> seperate;

	//vector<string>::iterator idx;
	size_t found;
	string tag_first_query, tag_second, tag_src;
	size_t pos = 0;

	int flag_close = -1;//flag close tag appear
	int flag_query = -1;//flag query appear

	bool flag = true;
	
	
	 int n,q;
	 cout << "Nhap vao so line source code: ";
	 cin >> n;
	 cout << "Nhap vao so queries: ";
	 cin >> q;
	 cout << "Nhap vao noi dung source code:" << endl;
	 cin.ignore();
	 for (int i=0; i<n; i++)
	 {
		 string a;
		 getline(cin,a);
		 src_code.push_back(a);
	 }

	 cout << "Nhap vao noi dung query:" << endl;
	 for (int i=0; i<q; i++)
	 {
		 string a;
		 getline(cin,a);
		 query.push_back(a);
	 }

	 cout << "***********************************" << endl;
	 cout << "********* VALID OF FIRST TAG*******" << endl;
	 cout << "***********************************" << endl;
	
	 /*
	 for (int i=0; i < query.size();i++)
	 {
		 bool result = true; 
		 bool flag = false;
		 result = header(src_code, query, i);
		 if (result == true)
		 {
			 size_t find_dot;
			 find_dot = query[i].find(DOT);
			 if (find_dot != std::string::npos)
			 {
				 flag = true;
			 }
			 else
			 {
				 flag = false;
			 }
		 }
		 else
		 {
			 cout << "Not found!" << endl;
		 }

		 if (flag == true)
		 {
			 string tag_first, tag_second;
			 size_t idx_first, idx_second;
			 idx_first = 
		 }
	 }
	 */
	 for (unsigned int i=0; i < query.size();i++)
	 {
		seperate = sequence_query(query[i]);
		printf_vector(seperate);
		flag = header(src_code,seperate[0]);
		if (flag == false)
		{
			cout << "Not found!" << endl;
			continue;
		}
		else
		{
			bool flag_seperate = true;
			for (int j=0; (j < seperate.size()) && (flag_seperate == true);j++)
			{				
				flag_seperate = compare_tag(src_code, seperate[j], seperate[j+1]);
			}
			if (flag_seperate == true)
			{

			}
			else
			{
			}
		}
	 }
    system("pause");
	}
 return 0;
}