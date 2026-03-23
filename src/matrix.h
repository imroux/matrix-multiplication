#ifndef MATRIX_H // Prevents duplicate includes
#define MATRIX_H

#include <string>
#include <vector>

// Function declarations (prototypes)
bool readMatrix(const std::string &filename,
                std::vector<std::vector<int>> &matrix, int &rows, int &cols);
void printMatrix(const std::string &name,
                 const std::vector<std::vector<int>> &matrix, int rows,
                 int cols);
bool multiplyMatrices(const std::vector<std::vector<int>> &A, int rowsA,
                      int colsA, const std::vector<std::vector<int>> &B,
                      int colsB, std::vector<std::vector<int>> &C,
                      int numThreads = 1);
// bool writeMatrix(const std::string &filename,
//                  const std::vector<std::vector<int>> &matrix, int rows,
//                  int cols);
bool appendToLog(const std::string &filename,
                 const std::vector<std::vector<int>> &matrix, int rows,
                 int cols, double computationTimeMs, int numThreads);
bool validateInputDimensions(int rowsA, int colsA, int rowsB, int colsB);

#endif // MATRIX_H
