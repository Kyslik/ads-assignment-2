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


inline bool fileExists (const string& file_name);


int main(int argc, const char * argv[])
{
    vector<vector<int> > matrix;

    if (!fileExists(MATRIX_FILE)) return -1;

    string line;
    ifstream inf;
    inf.open(MATRIX_FILE);
    if (!inf) return -1;

    while (getline(inf, line))
    {
        istringstream is( line );
        matrix.push_back(
                         vector<int>(istream_iterator<int>(is),
                                     istream_iterator<int>()
                                     )
                         );
    }

    int city_count = matrix[0][0];

    matrix.erase(matrix.begin());
    
    return 0;
}

inline bool fileExists (const string& file_name)
{
    struct stat buffer;
    return (stat (file_name.c_str(), &buffer) == 0);
}