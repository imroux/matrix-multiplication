#ifndef MATRIX_H
#define MATRIX_H

#include <string>
#include <vector>

// Function declarations (prototypes)
bool readMatrix(const std::string &filename,
                std::vector<std::vector<int>> &matrix, int &rows, int &cols);
void printMatrix(const std::string &name,
                 const std::vector<std::vector<int>> &matrix, int rows,
                 int cols);
bool validateInputDimensions(int rowsA, int colsA, int rowsB, int colsB);

#endif // MATRIX_H
