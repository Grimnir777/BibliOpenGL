#pragma once
#include <string>
#include <vector>
#include "sqlite3.h"
#include "Node.h"
using namespace std;

class Data
{
public:
	Data(string path);
	~Data();
	vector<string> get_first_ls();
	vector<string> get_first_2ls();
	vector<string> get_first_words();
	void fill_arrays(int year);
	void refill_arrays(int year);
	vector<string> first_ls;
	vector<string> first_2ls;
	vector<string> first_words;
	Node* rootNode = new Node();

private:
	sqlite3* DB;
	string dir;
	vector<vector<string>> resultsMulti;
	vector<string> results;
	

	void executeSQL(const char* sql);
	void executeSQLMulti(const char* sql);
};