import java.util.*;

public class Solution {
  public static long run(int[] scores, int k, int iters) {
    if (iters == 0) return 0L;
    int n = scores.length;
    Integer[] indices = new Integer[n];
    for (int i = 0; i < n; i++) indices[i] = i;
    Arrays.sort(indices, Comparator.comparingInt((Integer i) -> scores[i]).reversed()
                 .thenComparingInt(i -> i));
    long h = 0;
    for (int iter = 0; iter < iters; iter++) {
      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        int idx = indices[i];
        long score = scores[idx];
        h ^= (score << 32) ^ (idx & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}