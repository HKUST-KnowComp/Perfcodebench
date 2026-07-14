public class Solution {
  public static long run(int[] deltas, int iters) {
    long total = 0;
    int len = deltas.length;
    int unrollLimit = len - (len % 4);
    
    for (int iter = 0; iter < iters; iter++) {
      long current = 0;
      long sum = 0;
      
      int i = 0;
      for (; i < unrollLimit; i += 4) {
        current += deltas[i];
        sum += current + 0x9E3779B1L;
        
        current += deltas[i + 1];
        sum += current + 0x9E3779B1L;
        
        current += deltas[i + 2];
        sum += current + 0x9E3779B1L;
        
        current += deltas[i + 3];
        sum += current + 0x9E3779B1L;
      }
      
      for (; i < len; i++) {
        current += deltas[i];
        sum += current + 0x9E3779B1L;
      }
      
      total = sum;
    }
    return total;
  }
}