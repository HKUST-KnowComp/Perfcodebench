public class Solution {
  public static long run(int[] ids, int[] dictVals, int iters) {
    long h = 1469598103934665603L;
    int len = ids.length;
    
    // Pre-calculate the hash for a single pass to avoid redundant work if iters > 1
    // However, the requirement is to return the checksum after 'iters' iterations.
    // Since the 'decoded' array is overwritten with the same values every iteration,
    // the final hash 'h' is actually independent of the number of iterations 
    // as long as iters >= 1, because 'h' is reset to the seed inside the loop.
    
    for (int iter = 0; iter < iters; iter++) {
      h = 1469598103934665603L;
      for (int i = 0; i < len; i++) {
        long v = dictVals[ids[i]];
        h ^= (v & 0xffffffffL) + 1L;
        h *= 1099511628211L;
      }
    }
    return h;
  }
}