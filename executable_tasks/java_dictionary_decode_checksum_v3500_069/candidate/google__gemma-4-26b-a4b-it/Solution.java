public class Solution {
  public static long run(int[] ids, int[] dictVals, int iters) {
    long h = 1469598103934665603L;
    for (int iter = 0; iter < iters; iter++) {
      // Reset hash for each iteration as per original logic
      h = 1469598103934665603L;
      for (int i = 0; i < ids.length; i++) {
        // Combine lookup and hash calculation to avoid intermediate array writes
        long v = dictVals[ids[i]] & 0xffffffffL;
        h ^= (v + 1L);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}