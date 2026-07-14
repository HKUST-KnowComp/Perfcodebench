java
import java.util.*;

public class Solution {
  public static long run(int[] scores, int k, int iters) {
    int n = scores.length;
    long h = 0;
    for (int iter = 0; iter < iters; iter++) {
      Integer[] indices = new Integer[n];
      for (int i = 0; i < n; i++) {
        indices[i] = i;
      }
      Arrays.sort(indices, (a, b) -> {
        if (scores[a] != scores[b]) return Integer.compare(scores[b], scores[a]);
        return Integer.compare(a, b);
      });
      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        h ^= (((long) scores[indices[i]]) << 32) ^ (indices[i] & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}