#include "Data.h"
#include <iostream>
#include <fstream>
#include "sqlite3.h"
using namespace std;

Data::Data(string path)
{
	cout << "Data constructor called";
	this->dir = path;
    cout << this->dir <<"\n";
    int exit = 0;
    exit = sqlite3_open(this->dir.c_str(), &this->DB);
    if (exit) {
        std::cerr << "Error open DB " << sqlite3_errmsg(this->DB) << std::endl;
        return;
    }
    else
        std::cout << "Opened Database Successfully!" << std::endl;
    this->first_ls = this->get_first_ls();
    this->first_2ls = this->get_first_2ls();
    this->first_words = this->get_first_words();
}


Data::~Data()
{
    cout << "Data destructor called";
    sqlite3_close(this->DB);
}

static int callbackDB(void* ptr, int argc, char** argv, char** cols)
{
    vector<string>* vec = static_cast<vector<string>*>(ptr);
    vec->push_back(argv[0] ? argv[0] : "(NULL)");
    return 0;
}

void Data::executeSQL(const char* sql)
{
    char* zErrMsg = 0;
    int rc;
    const char* data = "Callback function called";

    /* Execute SQL statement */
    rc = sqlite3_exec(this->DB, sql, callbackDB, &this->results, &zErrMsg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else {
        fprintf(stdout, "Operation done successfully\n");
    }
}


vector<string> Data::get_first_ls()
{
    this->results.clear();
    const char* sql = R"""(
                select first_l from fos group by first_l order by first_l
                )""";

    this->executeSQL(sql);
    return this->results;
}

vector<string> Data::get_first_2ls()
{
    this->results.clear();
    const char* sql = R"""(
                select first_2l from fos group by first_2l order by first_2l
                )""";

    this->executeSQL(sql);
    return this->results;
}

vector<string> Data::get_first_words()
{
    this->results.clear();
    const char* sql = R"""(
                select first_word from fos group by first_word order by first_word
                )""";

    this->executeSQL(sql);
    return this->results;
}


/*
vector<string> Data::get_first_ls(int year)
{
    this->results.clear();
    string sql;
    const char* part1 = R"""(
                select first_l from fos 
                inner join fos_of_articles as fa on fos.name = fa.name 
                inner join articles as a on fa.article_id = a.id )""";
    string part2 = "where a.year = " + to_string(year) + " ";
    string part3 = "group by fos.first_l order by fos.first_l";
    sql = part1 + part2 + part3;
    this->executeSQL(sql.c_str());
    return this->results;
}

vector<string> Data::get_first_2ls(int year, string first_l)
{
    this->results.clear();
    string sql;
    const char* part1= R"""(
                select first_2l from fos 
                inner join fos_of_articles as fa on fos.name = fa.name 
                inner join articles as a on fa.article_id = a.id )""";
    string part2 = "where a.year = " + to_string(year) + " and fos.first_l like '" + first_l + "' ";
    string part3 = "group by fos.first_2l order by fos.first_2l";
    sql = part1 + part2 + part3;
    this->executeSQL(sql.c_str());
    return this->results;
}

vector<string> Data::get_first_words(int year, string first_2l)
{
    this->results.clear();
    string sql;
    const char* part1 = R"""(
                select fos.first_word from fos 
                inner join fos_of_articles as fa on fos.name = fa.name 
                inner join articles as a on fa.article_id = a.id )""";
    string part2 = "where a.year = " + to_string(year) + " and fos.first_2l like '" + first_2l + "' ";
    string part3 = "group by fos.first_word order by fos.first_word";
    sql = part1 + part2 + part3;
    this->executeSQL(sql.c_str());
    return this->results;
}
*/