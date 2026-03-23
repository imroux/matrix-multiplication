#include "matrix.h" // Include our own header first
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
      int value;
      if (file >> value) {
        // Successfully read an integer
        matrix[i][j] = value;
      } else {
        // Failed to read a value
        if (file.eof()) {
          cerr << "Unexpected end of file at row " + to_string(i + 1) +
                      ", col " + to_string(j + 1) + " in " + filename
               << endl;
          return false;
        } else if (file.fail()) {
          // Try to peek at the bad character for a more precise error
          file.clear(); // Clear fail state to peek
          char bad_char = file.peek();
          cerr << "Invalid number at row " + to_string(i + 1) + ", col " +
                      to_string(j + 1) + " (found '" + string(1, bad_char) +
                      "') in " + filename
               << endl;
          return false;
        }
        cerr << "Read error at row " + to_string(i + 1) + ", col " +
                    to_string(j + 1) + " in " + filename
             << endl;
        return false;
      }
    }
  }

  string leftover;
  if (file >> leftover) {
    cerr << "Extra data found after matrix in " + filename << endl;
    return false;
  }

  file.close(); // Possibly redundunt as ifstream destructor should
                // automatically handle cleanup
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

bool multiplyMatrices(const vector<vector<int>> &A, int rowsA, int colsA,
                      const vector<vector<int>> &B, int rowsB, int colsB,
                      vector<vector<int>> &C) {
  C.resize(rowsA, vector<int>(colsB, 0));

  // Standard matrix multiplication: C[i][j] = sum(A[i][k] * B[k][j])
  for (int i = 0; i < rowsA; ++i) {
    for (int j = 0; j < colsB; ++j) {
      for (int k = 0; k < colsA; ++k) {
        C[i][j] += A[i][k] * B[k][j];
      }
    }
  }
  return true;
}

bool writeMatrix(const string &filename, const vector<vector<int>> &matrix,
                 int rows, int cols) {
  ofstream file(filename);

  // Check if file opened successfully
  if (!file.is_open()) {
    cerr << "Error: Could not create file " << filename << endl;
    return false;
  }

  // Write dimensions on the first line
  file << rows << " " << cols << "\n";

  // Write matrix data row by row
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      file << matrix[i][j];
      // Add space between values except for the last one in a row
      if (j < cols - 1) {
        file << " ";
      }
    }
    file << "\n"; // Newline after each row
  }

  // Check if writing succeeded
  if (file.fail()) {
    cerr << "Error: Failed to write to " << filename << endl;
    file.close();
    return false;
  }

  file.close(); // Possibly redundunt as ofstream destructor should
                // automatically handle cleanup
  return true;
}

bool validateInputDimensions(int rowsA, int colsA, int rowsB, int colsB) {
  // Check that all matrix dimensions are positive numbers
  const char *dim_names[] = {"rowsA", "colsA", "rowsB", "colsB"};
  int dim_values[] = {rowsA, colsA, rowsB, colsB};

  for (int i = 0; i < 4; ++i) {
    if (dim_values[i] <= 0) {
      cerr << "Error: " << dim_names[i] << " (" << dim_values[i]
           << ") must be positive!" << endl;
      return false;
    }
  }

  // Check that matrix multiplication is possible
  if (colsA != rowsB) {
    cerr << "Error: colsA must be equal to rowsB!" << endl;
    return false;
  }

  return true;
}
