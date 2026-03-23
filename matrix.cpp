#include "matrix.h"  // Include our own header first
#include <algorithm> // Required for std::any_of
#include <fstream>
#include <iostream>

using namespace std;

bool readMatrix(const string &filename, vector<vector<int>> &matrix, int &rows,
                int &cols) {
  ifstream file(filename);

  if (!file.is_open()) {
    cerr << "Error: Could not open file " << filename << endl;
    return false;
  }

  if (!(file >> rows >> cols)) {
    cerr << "Error: Could not read dimensions from " << filename << endl;
    file.close();
    return false;
  }

  matrix.resize(rows, vector<int>(cols));

  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      if (!(file >> matrix[i][j])) {
        cerr << "Error: Not enough data in " << filename << endl;
        file.close();
        return false;
      }
    }
  }

  file.close();
  return true;
}

void printMatrix(const string &name, const vector<vector<int>> &matrix,
                 int rows, int cols) {
  cout << "Matrix " << name << " (" << rows << "x" << cols << "):" << endl;
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      cout << matrix[i][j] << "\t";
    }
    cout << endl;
  }
  cout << endl;
}

bool validateInputs(int rowsA, int colsA, int rowsB, int colsB) {
  // Check that all matrix dimensions are positive numbers
  int dims[] = {rowsA, colsA, rowsB, colsB};
  if (any_of(begin(dims), end(dims), [](int val) { return val <= 0; })) {
    cerr << "Error: Matrix dimensions must be positive numbers!" << endl;
    return false;
  }

  // Check that matrix multiplication is possible
  if (colsA != rowsB) {
    cerr << "Error: colsA must be equal to rowsB!" << endl;
    return false;
  }

  return true;
}
