#include "Data.h"
#include <iostream>
#include <fstream>
#include "sqlite3.h"
using namespace std;

Data::Data(string path)
{
	cout << "Data constructor called";
	this->dir = path.c_str();
    sqlite3* DB;
    int exit = 0;
    exit = sqlite3_open(dir, &DB);
    if (exit) {
        std::cerr << "Error open DB " << sqlite3_errmsg(DB) << std::endl;
        return;
    }
    else
        std::cout << "Opened Database Successfully!" << std::endl;
    sqlite3_close(DB);
}

vector<string> Data::get_first_ls(int year)
{
    vector<string> vec;
    return vec;
}

vector<string> Data::get_first_2ls(int year, string first_l)
{
    vector<string> vec;
    return vec;
}

vector<string> Data::get_first_words(int year, string first_2l)
{
    vector<string> vec;
    return vec;
}