#include "matrix.h" // Include custom matrix operations
#include <iostream>

using namespace std;

int main() {
  vector<vector<int>> matrixA;
  vector<vector<int>> matrixB;
  int rowsA, colsA;
  int rowsB, colsB;

  // Read Matrix A
  if (!readMatrix("A.txt", matrixA, rowsA, colsA)) {
    return 1;
  }

  // Read Matrix B
  if (!readMatrix("B.txt", matrixB, rowsB, colsB)) {
    return 1;
  }

  // Validate correctness of input matrices
  if (!validateInputs(rowsA, colsA, rowsB, colsB)) {
    return 1;
  }

  // Display the loaded matrices
  printMatrix("A", matrixA, rowsA, colsA);
  printMatrix("B", matrixB, rowsB, colsB);

  cout << "Matrices loaded successfully." << endl;

  return 0;
}
