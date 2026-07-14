public class Solution {
  public static long run(int[] deltas, int iters) {
    final int n = deltas.length;
    if (n == 0) return 0;
    
    final long constantPart = (long) n * 0x9E3779B1L;
    long total = 0;

    for (int iter = 0; iter < iters; iter++) {
      long current = 0;
      long sum = 0;
      
      // Unroll the loop to reduce overhead and improve ILP
      int i = 0;
      for (; i <= n - 4; i += 4) {
        long d0 = deltas[i];
        long d1 = deltas[i + 1];
        long d2 = deltas[i + 2];
        long d3 = deltas[i + 3];
        
        long c0 = current + d0;
        long c1 = c0 + d1;
        long c2 = c1 + d2;
        long c3 = c2 + d3;
        
        sum += (c0 + c1 + c2 + c3);
        current = c3;
      }
      
      // Handle remaining elements
      for (; i < n; i++) {
        current += deltas[i];
        sum += current;
      }
      
      total = sum + constantPart;
    }
    
    return total;
  }
}