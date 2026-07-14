public class Solution {
  public static long run(int[] ids, int[] dictVals, int iters) {
    long h = 1469598103934665603L;
    int len = ids.length;
    
    for (int iter = 0; iter < iters; iter++) {
      // Reset hash for each iteration as per original logic
      long currentH = 1469598103934665603L;
      for (int i = 0; i < len; i++) {
        // Direct lookup and hash update to avoid intermediate array writes/reads
        int v = dictVals[ids[i]];
        currentH ^= (v & 0xffffffffL) + 1L;
        currentH *= 1099511628211L;
      }
      h = currentH;
    }
    return h;
  }
}