#pragma once
#include <string>
#include <vector>
#include "sqlite3.h"
using namespace std;

class Data
{
public:
	Data(string path);
	~Data();
	vector<string> get_first_ls();
	vector<string> get_first_2ls();
	vector<string> get_first_words();
	vector<string> first_ls;
	vector<string> first_2ls;
	vector<string> first_words;
private:
	sqlite3* DB;
	string dir;
	vector<string> results;

	void executeSQL(const char* sql);
};