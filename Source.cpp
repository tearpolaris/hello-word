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

		//bool bQuery;

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
		for (unsigned int i=0; i < query.size();i++)
		{
			seperate = sequence_query(query[i]);
			printf_vector(seperate);
			flag = header(src_code,seperate[0]);
			if (flag == false)
			{
				cout << "Not found!" << endl;
				//continue;
			}
			else
			{
				cout << "Found" << endl;
				flag = result_query(src_code,seperate);

				if (flag)
				{
					cout << "Successfully!" << endl;
				}
				else
				{
					cout << "Fail" << endl;
				}
			}
		}	
		system("pause");
	}
	return 0;
}