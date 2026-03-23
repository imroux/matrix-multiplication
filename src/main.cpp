#include "matrix.h" // Include custom matrix operations
#include <chrono>
#include <iostream>

using namespace std;

int main() {
  vector<vector<int>> matrixA;
  vector<vector<int>> matrixB;
  vector<vector<int>> matrixC;
  int rowsA, colsA;
  int rowsB, colsB;
  int p_threads = 6;

  // Read Matrix A
  if (!readMatrix("data/A.txt", matrixA, rowsA, colsA)) {
    return 1;
  }

  // Read Matrix B
  if (!readMatrix("data/B.txt", matrixB, rowsB, colsB)) {
    return 1;
  }

  // Validate correctness of input matrices
  if (!validateInputDimensions(rowsA, colsA, rowsB, colsB)) {
    return 1;
  }

  // Display the loaded matrices
  printMatrix("A", matrixA, rowsA, colsA);
  printMatrix("B", matrixB, rowsB, colsB);

  cout << "Matrices loaded successfully." << endl << endl;

  // Measure computation time for matrix multiplication
  auto startTime = chrono::high_resolution_clock::now();

  // Multiply matrices A & B
  if (!multiplyMatrices(matrixA, rowsA, colsA, matrixB, colsB, matrixC,
                        p_threads)) {
    return 1;
  }

  auto endTime = chrono::high_resolution_clock::now();
  double computationTimeMs =
      chrono::duration<double, milli>(endTime - startTime).count();

  // Display resulting C matrix
  printMatrix("A * B", matrixC, rowsA, colsB);

  // Write resulting matrix into C.txt
  // if (!writeMatrix("data/C.txt", matrixC, rowsA, colsB)) {
  //   return 1;
  // }

  // Launch log writer thread
  appendToLog("data/matrix_multiplication.log", matrixC, rowsA, colsB,
              computationTimeMs, p_threads);
  // if (!appendToLog("data/matrix_multiplication.log", matrixC, rowsA, colsB))
  // {
  //   return 1;
  // }

  return 0;
}
