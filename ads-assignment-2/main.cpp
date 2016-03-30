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
#include <list>

#include <sys/stat.h>
#include <unistd.h>

using namespace std;

const string MATRIX_FILE = "input.txt";

typedef vector<vector<unsigned short>> Matrix;
typedef vector<unsigned short> *oneDMat;

inline bool fileExists (const string& file_name);
bool readMatrix(Matrix &m, const short size, const string &file_name = MATRIX_FILE);
void subMatrix(oneDMat submat, const Matrix &mat);
list<int> minC(oneDMat submat, const Matrix &mat);

int main(int argc, const char * argv[])
{
    if (argc != 2) return -1;
    int mat_size = atoi(argv[1]);
    Matrix mat;
    oneDMat submat;
    readMatrix(mat, mat_size);
    
    size_t n = mat.size();
    submat = new vector<unsigned short>((1 << n) * n, SHRT_MAX);
    subMatrix(submat, mat);
    list<int> cycle;
    unsigned short cycle_mat = 0;
    cycle = minC(submat, mat);

    list<int>::const_iterator it;

    for (it = cycle.begin(); it != cycle.end(); ++it) {
        cout << *it + 1 << endl;
        if (it != cycle.begin())
            cycle_mat += mat[*it][*prev(it)];
    }
    cout << cycle_mat << endl;
    return 0;
}

void subMatrix(oneDMat submat, const Matrix &mat)
{
    int n = (int) mat.size();
    size_t nset = 1 << n;
    (*submat)[1 * n + 0] = 0;

    for (size_t s = 3; s < nset; s += 2)
    {
        for (int j = 1; j < n; j++)
        {
            if (!(s & (1 << j)))
                continue;
            size_t t = s & ~(1 << j);

            for (int i = 0; i < n; i++)
            {
                if (s & (1 << i) && i != j && (*submat)[t * n + i] < SHRT_MAX)
                    (*submat)[s * n + j] = min((*submat)[s * n +j], (unsigned short) ((*submat)[t * n + i] + mat[i][j]));
            }
        }
    }
}

list<int> minC(oneDMat submat, const Matrix &mat)
{
    list<int> cycle(1, 0);
    int n = (int) mat.size();
    size_t nset = 1 << n;
    vector<bool> visited(n, false);

    size_t s = nset - 1;

    visited[0] = true;

    for (int i = 0; i < n - 1; i++)
    {
        int best_j = 0;
        double min_mat = SHRT_MAX;

        for (int j = 0; j < n; j++)
        {
            if (!visited[j] && (*submat)[s * n +j] + mat[cycle.back()][j] < min_mat)
            {
                min_mat = (*submat)[s * n + j] + mat[cycle.back()][j];
                best_j = j;
            }
        }

        cycle.push_back(best_j);
        visited[best_j] = true;
        s &= ~(1 << best_j);
    }
    
    cycle.push_back(0);
    return cycle;
}

bool readMatrix(Matrix &m, const short max_matrix_size, const string &file_name)
{
    if (!fileExists(file_name)) return false;
    string line;
    ifstream inf;

    inf.open(file_name);
    if (!inf) return false;
    short counter = 0;
    while (getline(inf, line))
    {
        if (counter > max_matrix_size) break;
        istringstream is( line );
        m.push_back(
                    vector<unsigned short>(istream_iterator<unsigned short>(is),
                                istream_iterator<unsigned short>()
                                )
                    );
        counter++;
    }

    m.erase(m.begin());

    for (auto &n : m)
    {
        for (;n.size() > max_matrix_size;) {
            n.pop_back();
        }
    }
    return true;
}

inline bool fileExists (const string& file_name)
{
    struct stat buffer;
    return (stat (file_name.c_str(), &buffer) == 0);
}