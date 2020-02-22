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
	void callback(void* data, int argc, char** argv, char** azColName);
	string dir;
	vector<string> results;
	void executeSQL(const char* sql);
};