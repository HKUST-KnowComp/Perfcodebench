public class Solution {
  public static long run(int[] ids, int[] dictVals, int iters) {
    long h = 1469598103934665603L;
    for (int iter = 0; iter < iters; iter++) {
      // Reset hash for each iteration as per original logic
      h = 1469598103934665603L;
      for (int i = 0; i < ids.length; i++) {
        // Directly fetch from dictVals using ids[i] and update hash
        // This avoids the overhead of writing to and reading from the 'decoded' array
        long v = dictVals[ids[i]] & 0xffffffffL;
        h ^= (v + 1L);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}