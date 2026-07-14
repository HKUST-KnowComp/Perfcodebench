public class Solution {
  public static long run(int[] ids, int[] dictVals, int iters) {
    long h = 1469598103934665603L;
    int len = ids.length;
    
    // Pre-calculate the checksum for the first iteration to establish the base
    // However, the problem states 'h' is reset to the seed every iteration.
    // The loop structure implies 'h' is the result of the LAST iteration.
    
    for (int iter = 0; iter < iters; iter++) {
      h = 1469598103934665603L;
      for (int i = 0; i < len; i++) {
        int v = dictVals[ids[i]];
        h ^= (v & 0xffffffffL) + 1L;
        h *= 1099511628211L;
      }
    }
    return h;
  }
}