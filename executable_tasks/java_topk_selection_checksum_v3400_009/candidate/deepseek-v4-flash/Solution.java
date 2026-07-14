import java.util.*;

public class Solution {
  public static long run(int[] scores, int k, int iters) {
    int n = scores.length;
    // Precompute sorted indices: sort by score descending, then index ascending.
    Integer[] indices = new Integer[n];
    for (int i = 0; i < n; i++) indices[i] = i;
    Arrays.sort(indices, (i, j) -> {
      if (scores[i] != scores[j]) return Integer.compare(scores[j], scores[i]);
      return Integer.compare(i, j);
    });
    // Convert to int[] for faster access
    int[] sortedIndices = new int[n];
    for (int i = 0; i < n; i++) sortedIndices[i] = indices[i];

    long h = 0;
    for (int iter = 0; iter < iters; iter++) {
      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        int idx = sortedIndices[i];
        long v = ((long) scores[idx]) << 32;
        v ^= (idx & 0xffffffffL);
        h ^= v;
        h *= 1099511628211L;
      }
    }
    return h;
  }
}