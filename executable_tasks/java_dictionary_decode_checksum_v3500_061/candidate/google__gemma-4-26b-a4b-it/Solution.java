public class Solution {
  public static long run(int[] ids, int[] dictVals, int iters) {
    long h = 1469598103934665603L;
    // The original code resets h = 1469598103934665603L at the start of every iteration.
    // Therefore, the result of the function is solely determined by the final iteration.
    // We can skip the first (iters - 1) iterations entirely.
    if (iters <= 0) return 0; // Handle edge case if iters could be 0

    for (int i = 0; i < ids.length; i++) {
      int v = dictVals[ids[i]];
      h ^= (v & 0xffffffffL) + 1L;
      h *= 1099511628211L;
    }
    return h;
  }
}