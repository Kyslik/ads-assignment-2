//
//  main.cpp
//  ads-assignment-2
//
//  Created by Martin Kiesel on 28/03/16.
//  Copyright © 2016 Martin Kiesel. All rights reserved.

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <sys/stat.h>
#include <unistd.h>

using namespace std;

const string MATRIX_FILE = "input-min.txt";

typedef vector<vector<int>> matrix;

inline bool fileExists (const string& file_name);
bool readMatrix(matrix &m, const string &file_name = MATRIX_FILE);

int main(int argc, const char * argv[])
{
    matrix matrix;
    if (!readMatrix(matrix)) return 1;

    int city_count = matrix[0][0];

    matrix.erase(matrix.begin());
    
    return 0;
}

bool readMatrix(matrix &m, const string &file_name)
{
    if (!fileExists(file_name)) return false;

    string line;
    ifstream inf;

    inf.open(file_name);
    if (!inf) return false;

    while (getline(inf, line))
    {
        istringstream is( line );
        m.push_back(
                    vector<int>(istream_iterator<int>(is),
                                istream_iterator<int>()
                                )
                    );
    }
    return true;
}

inline bool fileExists (const string& file_name)
{
    struct stat buffer;
    return (stat (file_name.c_str(), &buffer) == 0);
}