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
typedef list<int> Cycle;

inline bool fileExists (const string& file_name);
bool readMatrix(Matrix &m, const short size, const string &file_name = MATRIX_FILE);
void subMatrix(oneDMat submat, const Matrix &mat);
void displayCycle(const Matrix &mat, Cycle cycle);
int countCycle(const Matrix &mat, Cycle cycle);

Cycle minC(oneDMat submat, const Matrix &mat);
Cycle greedIt(const Matrix &mat);
Cycle swapCities(const Cycle &cycle, const int &a, const int &b);
Cycle twoOptAllowNeg(const Matrix &mat, Cycle cycle, int default_neg, int diff_d, int min);

int main(int argc, const char * argv[])
{
    int mat_size = 40;
    Matrix mat;
    Cycle cycle;

    if (argc == 1)
    {
        readMatrix(mat, mat_size);
        cycle = greedIt(mat);
        cycle = twoOptAllowNeg(mat, cycle, 6, 300, 9000);
        cycle = twoOptAllowNeg(mat, cycle, 3, 250, 8027);
    }
    else if (argc == 2)
    {
        mat_size = atoi(argv[1]);
        if (mat_size > 30 || 0 >= mat_size) return -1;
        oneDMat submat;
        readMatrix(mat, mat_size);
        submat = new vector<unsigned short>((1 << mat_size) * mat_size, SHRT_MAX);

        subMatrix(submat, mat);
        cycle = minC(submat, mat);
    }
    else
        return -1;

    displayCycle(mat, cycle);
    cout << countCycle(mat, cycle) << endl;

    return 0;
}

Cycle greedIt(const Matrix &mat)
{
    size_t n = mat.size();
    Cycle cycle;
    vector<bool> visited(n, false);

    visited[0] = true;
    int i = 0;

    while (cycle.size() != n)
    {
        int best_j = 0;
        int best_val = INT32_MAX;
        for (int j = 0; j < n; j++) {
            if (visited[j] || best_val < mat[i][j]) continue;
            best_j = j;
            best_val = mat[i][j];
        }
        cycle.push_back(best_j);
        visited[best_j] = true;
        i = best_j;
    }

    cycle.push_front(0);
    return cycle;
}

Cycle swapCities(const Cycle &cycle, const int &a, const int &b)
{
    Cycle n_cycle;
    size_t cycle_size = cycle.size();
    Cycle::const_iterator it;
    vector<int> vcycle;

    for (const auto &c : cycle)
        vcycle.push_back(c);

    for ( int c = 0; c <= a - 1; ++c )
        n_cycle.push_back(vcycle[c]);

    for (int c = a, dec = 0; c <= b; ++c, dec++)
        n_cycle.push_back(vcycle[b - dec]);

    for ( int c = b + 1; c < cycle_size; ++c )
        n_cycle.push_back(vcycle[c]);

    return n_cycle;
}

Cycle twoOptAllowNeg(const Matrix &mat, Cycle cycle, int default_neg, int diff_d, int min)
{
    size_t cycle_size = cycle.size();
    int improve = 0;
    bool distance_treshold = false;

    while (improve < 10)
    {
        int neg_imp = default_neg;
        int best_distance = countCycle(mat, cycle);

        for (int i = 1; i < cycle_size - 1;  i++)
        {
            for (int j = i + 1; j < cycle_size - 1; j++)
            {
                Cycle new_cycle = swapCities(cycle, i, j);
                int new_distance = countCycle(mat, new_cycle);
                int diff_distance = new_distance - best_distance;
                if (distance_treshold)
                {
                    if ((new_distance < min) && (new_distance < best_distance))
                    {
                        improve = 0;
                        neg_imp--;
                        cycle = new_cycle;
                        best_distance = new_distance;
                    }
                }
                else
                {
                    if (new_distance < best_distance || (diff_distance < diff_d && neg_imp > 0))
                    {
                        improve = 0;
                        neg_imp--;
                        cycle = new_cycle;
                        best_distance = new_distance;
                        if (new_distance < min) distance_treshold = true;
                    }

                }
            }
        }
        improve++;
    }
    return cycle;
}

void displayCycle(const Matrix &mat, Cycle cycle)
{
    Cycle::const_iterator it;

    for (it = cycle.begin(); it != cycle.end(); ++it)
        cout << *it + 1 << endl;
}

int countCycle(const Matrix &mat, Cycle cycle)
{
    Cycle::const_iterator it;
    int cycle_mat = 0;
    for (it = cycle.begin(); it != cycle.end(); ++it)
    {
        if (it != cycle.begin())
            cycle_mat += mat[*it][*prev(it)];
    }

    return cycle_mat;
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
                    (*submat)[s * n + j] = min((*submat)[s * n + j], (unsigned short) ((*submat)[t * n + i] + mat[i][j]));
            }
        }
    }
}

Cycle minC(oneDMat submat, const Matrix &mat)
{
    Cycle cycle(1, 0);
    size_t n = mat.size();
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