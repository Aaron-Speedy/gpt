// A: K x M
// B: N x K
kernel void matrix_multiply(const int M, const int N, const int K,
            const global float *A,
            const global float *B,
            global float *C) {
  // Thread identifiers
  const int global_row = get_global_id(0); // Row ID of C (0..M)
  const int global_col = get_global_id(1); // Col ID of C (0..N)
 
  // Compute a single element (loop over K)
  float acc = 0.0f;
  for (int k = 0; k < K; k++) {
    acc += A[k*M + global_row] * B[global_col*K + k];
  }
 
  // Store the result
  C[global_col*M + global_row] = acc;
}
