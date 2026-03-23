#ifndef MATRIX_H // Prevents duplicate includes
#define MATRIX_H

#include <string>
#include <vector>

using namespace std;

// Function declarations (prototypes)
bool readMatrix(const string &filename, vector<vector<int>> &matrix, int &rows,
                int &cols);
void printMatrix(const string &name, const vector<vector<int>> &matrix,
                 int rows, int cols);
bool validateInputDimensions(int rowsA, int colsA, int rowsB, int colsB);

#endif // MATRIX_H
