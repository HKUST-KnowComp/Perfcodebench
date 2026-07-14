import java.util.*;

public class Solution {
  private static final long FNV_OFFSET = 1469598103934665603L;
  private static final long FNV_PRIME = 1099511628211L;

  public static long run(int[] scores, int k, int iters) {
    final int n = scores.length;
    if (k <= 0) {
      long h = 0;
      for (int iter = 0; iter < iters; iter++) h = FNV_OFFSET;
      return h;
    }
    if (k > n) k = n;

    long[] top = new long[k];
    long h = 0L;

    for (int iter = 0; iter < iters; iter++) {
      int size = 0;
      long worstKey = 0L;
      int worstPos = -1;

      for (int i = 0; i < n; i++) {
        long key = pack(scores[i], i);
        if (size < k) {
          top[size++] = key;
          if (size == k) {
            worstPos = 0;
            worstKey = top[0];
            for (int j = 1; j < k; j++) {
              long v = top[j];
              if (v > worstKey) {
                worstKey = v;
                worstPos = j;
              }
            }
          }
        } else if (key < worstKey) {
          top[worstPos] = key;
          worstPos = 0;
          worstKey = top[0];
          for (int j = 1; j < k; j++) {
            long v = top[j];
            if (v > worstKey) {
              worstKey = v;
              worstPos = j;
            }
          }
        }
      }

      Arrays.sort(top, 0, k);

      h = FNV_OFFSET;
      for (int i = 0; i < k; i++) {
        long key = top[i];
        int score = unpackScore(key);
        int index = unpackIndex(key);
        h ^= (((long) score) << 32) ^ (index & 0xffffffffL);
        h *= FNV_PRIME;
      }
    }
    return h;
  }

  private static long pack(int score, int index) {
    return (((long) ~score) << 32) | (index & 0xffffffffL);
  }

  private static int unpackScore(long key) {
    return ~((int) (key >>> 32));
  }

  private static int unpackIndex(long key) {
    return (int) key;
  }
}
