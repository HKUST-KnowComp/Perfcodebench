public class Solution {
  public static long run(int[] deltas, int iters) {
    long total = 0;
    int len = deltas.length;
    int limit = len - (len % 4);
    
    for (int iter = 0; iter < iters; iter++) {
      long current = 0;
      long sum = 0;
      
      // Unrolled loop for main body
      for (int i = 0; i < limit; i += 4) {
        current += deltas[i];
        sum += current + 0x9E3779B1L;
        
        current += deltas[i + 1];
        sum += current + 0x9E3779B1L;
        
        current += deltas[i + 2];
        sum += current + 0x9E3779B1L;
        
        current += deltas[i + 3];
        sum += current + 0x9E3779B1L;
      }
      
      // Handle remaining elements
      for (int i = limit; i < len; i++) {
        current += deltas[i];
        sum += current + 0x9E3779B1L;
      }
      
      total = sum;
    }
    return total;
  }
}