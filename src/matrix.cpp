#include "matrix.h" // Include our own header first
#include <fstream>
#include <iostream>
#include <pthread.h>
#include <unistd.h> // for usleep()

using namespace std;

// Shared state for work distribution
// Struct is required for packaging of data due to the fact pthread_create
// accepts only one pointer
struct MultiplyTask {
  const vector<vector<int>> *A; // Need to read A[i][k]
  const vector<vector<int>> *B; // Need to read B[k][j]
  vector<vector<int>> *C;       // Need to write C[i][j]
  int colsA;                    // For inner loop: k < colsA
  int colsB;                    // To know when j wraps
  int *next_i;                  // Shared work tracker
  int *next_j;                  // Shared work tracker
  int *remaining;               // To know when to stop
  pthread_mutex_t *mutex;       // Mutex for synchronization
};

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

// Worker thread: computes one element at a time
// This code runs in parallel, in a separate thread
// Each thread executes this same function
void *multiplyWorker(void *arg) {
  // static_cast is required due to the fact that pthread_create only accepts
  // void* (generic pointer) while we need to pass MultiplyTask* struct. The
  // static_cast converts it back.
  MultiplyTask *task = static_cast<MultiplyTask *>(arg);

  int sum = 0;

  while (true) {
    int i, j;

    // Claim next uncomputed element (critical section)
    pthread_mutex_lock(task->mutex);

    // Break out of the loop if no more work left
    if (*task->remaining == 0) {
      pthread_mutex_unlock(task->mutex);
      break;
    }

    i = *task->next_i;
    j = *task->next_j;

    // Advance to the next position
    (*task->next_j)++;
    if (*task->next_j >= task->colsB) {
      *task->next_j = 0;
      (*task->next_i)++;
    }
    (*task->remaining)--;

    pthread_mutex_unlock(task->mutex);

    // Compute C[i][j] = sum(A[i][k] * B[k][j])
    sum = 0;
    for (int k = 0; k < task->colsA; ++k) {
      sum += (*task->A)[i][k] * (*task->B)[k][j];
    }
    (*task->C)[i][j] = sum;

    // Small delay to allow thread interleaving (per requirements)
    usleep(1000); // 1ms
  }

  // Required by the POSIX threads API
  return nullptr;
}

bool multiplyMatrices(const vector<vector<int>> &A, int rowsA, int colsA,
                      const vector<vector<int>> &B, int rowsB, int colsB,
                      vector<vector<int>> &C, int numThreads) {
  if (numThreads < 1)
    numThreads = 1;

  // Initialize result matrix
  C.resize(rowsA, vector<int>(colsB, 0));

  // Single-threaded fallback (original logic)
  if (numThreads == 1) {
    for (int i = 0; i < rowsA; ++i) {
      for (int j = 0; j < colsB; ++j) {
        for (int k = 0; k < colsA; ++k) {
          C[i][j] += A[i][k] * B[k][j];
        }
      }
    }
    return true;
  }

  // Static mutex initialization (no function call to pthread_mutex_init()
  // needed)
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

  int next_i = 0, next_j = 0;
  int remaining = rowsA * colsB;

  MultiplyTask task;
  task.A = &A;
  task.B = &B;
  task.C = &C;
  task.colsA = colsA;
  task.colsB = colsB;
  task.next_i = &next_i;
  task.next_j = &next_j;
  task.remaining = &remaining;
  task.mutex = &mutex;

  // Create worker threads
  vector<pthread_t> threads(numThreads);
  for (int t = 0; t < numThreads; ++t) {
    pthread_create(&threads[t], nullptr, multiplyWorker, &task);
  }

  // Wait for all threads to finish
  for (int t = 0; t < numThreads; ++t) {
    pthread_join(threads[t], nullptr);
  }

  pthread_mutex_destroy(&mutex);
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
