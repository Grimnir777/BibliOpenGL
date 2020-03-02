#include "Data.h"
#include <iostream>
#include <fstream>
#include <map>

Data::Data(string path)
{
    cout << "Data Initialization" << endl;
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
    
    this->fill_arrays(2015);
    this->first_ls = this->get_first_ls();
    this->first_2ls = this->get_first_2ls();
    this->first_words = this->get_first_words();

    //std::cout << "Size 1st " << this->first_ls.size() << std::endl;
    //std::cout << "Size 2nd " << this->first_2ls.size() << std::endl;
    //std::cout << "Size words " << this->first_words.size() << std::endl;

    /*
        0 : fos name
        1 : first_l 
        2 : first_2l
        3 : first_w
        4 : count of fos
    */
    
    std::map<std::string, std::string> m;
    for (int i = 0; i < this->resultsMulti.size(); i++)
    {
        m[this->resultsMulti[i][3]] = this->resultsMulti[i][4];
    }
    

    // Create tree
    // Loop with every first letter
    for (string first_l : this->first_ls)
    {
        // random color for every sub nodes
        glm::vec3 randColor = glm::vec3(
            static_cast <float> (rand()) / static_cast <float> (RAND_MAX),
            static_cast <float> (rand()) / static_cast <float> (RAND_MAX),
            static_cast <float> (rand()) / static_cast <float> (RAND_MAX));

        Node* flNode = new Node(randColor, first_l);

        // Loop with every first and second letter
        for (string first_2l : this->first_2ls)
        {
            if (first_l.compare(first_2l.substr(0, 1)) == 0) {
                //std::cout << first_2l.substr(0, 1) << endl;
                Node* f2lNode = new  Node(randColor, first_2l);
                
                // Loop with every first word
                for (string first_word : this->first_words) {
                    if (first_2l.compare(first_word.substr(0, 2)) == 0) {
                        Node* wordNode = new Node(randColor, first_word, m[first_word]);
                        f2lNode->AddChild(wordNode);
                    }
                }
                
                flNode->AddChild(f2lNode);
            }
        }
        this->rootNode->AddChild(flNode);
    }
    cout << "Data Initialization succeed" << endl;
}

void Data::refill_arrays(int year) {
    this->rootNode = new Node();
    cout << "refill asked for year " << year << endl;
    this->fill_arrays(year);
    std::map<std::string, std::string> m;
    for (int i = 0; i < this->resultsMulti.size(); i++)
    {
        m[this->resultsMulti[i][3]] = this->resultsMulti[i][4];
    }

    // Create tree
    // Loop with every first letter
    for (string first_l : this->first_ls)
    {
        // random color for every sub nodes
        glm::vec3 randColor = glm::vec3(
            static_cast <float> (rand()) / static_cast <float> (RAND_MAX),
            static_cast <float> (rand()) / static_cast <float> (RAND_MAX),
            static_cast <float> (rand()) / static_cast <float> (RAND_MAX));

        Node* flNode = new Node(randColor, first_l);

        // Loop with every first and second letter
        for (string first_2l : this->first_2ls)
        {
            if (first_l.compare(first_2l.substr(0, 1)) == 0) {
                //std::cout << first_2l.substr(0, 1) << endl;
                Node* f2lNode = new  Node(randColor, first_2l);

                // Loop with every first word
                for (string first_word : this->first_words) {
                    if (first_2l.compare(first_word.substr(0, 2)) == 0) {
                        Node* wordNode = new Node(randColor, first_word, m[first_word]);
                        f2lNode->AddChild(wordNode);
                    }
                }

                flNode->AddChild(f2lNode);
            }
        }
        this->rootNode->AddChild(flNode);
    }
    cout << "Data Initialization succeed" << endl;
}


Data::~Data()
{
    cout << "Data destructor called";
    sqlite3_close(this->DB);
}

// With multi results
static int callbackDBMulti(void* ptr, int argc, char** argv, char** cols)
{
    vector<vector<string>>* vec = static_cast<vector<vector<string>>*>(ptr);

    vector<string> row;
    for (int i = 0; i < argc; i++)
        row.push_back(argv[i] ? argv[i] : "(NULL)");
    vec->push_back(row);
    return 0;
}

void Data::executeSQLMulti(const char* sql)
{
    char* zErrMsg = 0;
    int rc;
    const char* data = "Callback function called";

    /* Execute SQL statement */
    rc = sqlite3_exec(this->DB, sql, callbackDBMulti, &this->resultsMulti, &zErrMsg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else {
        fprintf(stdout, "Operation done successfully\n");
    }
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

void Data::fill_arrays(int year)
{
    this->results.clear();
    string sql;
    const char* part1 = R"""(
                select fos.name, fos.first_l, first_2l, first_word, count(*) from fos 
                inner join fos_of_articles as fa on fos.name = fa.name 
                inner join articles as a on fa.article_id = a.id )""";
    string part2 = "where a.year = " + to_string(year) + " ";
    string part3 = "group by fos.first_word order by fos.first_word";
    sql = part1 + part2 + part3;
    this->executeSQLMulti(sql.c_str());
}
