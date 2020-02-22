#pragma once
#include <string>
#include <vector>
using namespace std;

class Data
{
public:
	Data(string path);
	vector<string> get_first_ls(int year);
	vector<string> get_first_2ls(int year, string first_l);
	vector<string> get_first_words(int year, string first_2l);
private:
	const char* dir;
};