import java.util.*;

public class Solution {
  public static long run(int[] scores, int k, int iters) {
    int n = scores.length;
    // Precompute sorted order of indices once
    Integer[] idxArr = new Integer[n];
    for (int i = 0; i < n; i++) idxArr[i] = i;
    Arrays.sort(idxArr, (a, b) -> {
      int cmp = Integer.compare(scores[b], scores[a]); // descending score
      if (cmp != 0) return cmp;
      return Integer.compare(a, b); // ascending index
    });
    // Convert to primitive int[] for fast access
    int[] sortedIdx = new int[n];
    for (int i = 0; i < n; i++) sortedIdx[i] = idxArr[i];
    // Compute checksum iters times
    long h = 0;
    int lim = Math.min(k, n);
    for (int iter = 0; iter < iters; iter++) {
      h = 1469598103934665603L;
      for (int i = 0; i < lim; i++) {
        int idx = sortedIdx[i];
        int score = scores[idx];
        h ^= (((long) score) << 32) ^ (idx & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}