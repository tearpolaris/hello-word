#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;

int main()
{
  int n,q;

  vector<string> src_code;
  vector<string> find_query;
  vector<string>::iterator idx;

  cout << "Nhap so dong code: ";
  cin >> n;
  cout << "Nhap so queries: ";
  cin >> q;
  cout << "Nhap vao source code: " << endl;
  for (int i=0; i<n; i++)
  {
    string a;
    getline(cin,a);
    src_code.push_back(a);
  }

  for (int i=0; i<q; i++)
  {
    string a;
    getline(cin,a);
    find_query.push_back(a);
  }

  for (idx = src_code.begin(); idx != src_code.end(); idx++)
  {
    cout << *idx << endl;
  }
  
  return 0;
}
