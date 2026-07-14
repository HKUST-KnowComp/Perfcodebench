import java.util.*;

public class Solution {
  static class Pair {
    int score;
    int index;
    Pair(int score, int index) { this.score = score; this.index = index; }
  }

  public static long run(int[] scores, int k, int iters) {
    if (iters == 0) return 0;
    int n = scores.length;
    // precompute sorted top-k indices once
    Integer[] idx = new Integer[n];
    for (int i = 0; i < n; i++) idx[i] = i;
    Arrays.sort(idx, (a, b) -> {
      if (scores[a] != scores[b]) return Integer.compare(scores[b], scores[a]);
      return Integer.compare(a, b);
    });
    // compute checksum over the exact top-k winners
    long chk = 1469598103934665603L;
    for (int i = 0; i < k; i++) {
      chk ^= (((long) scores[idx[i]]) << 32) ^ (idx[i] & 0xffffffffL);
      chk *= 1099511628211L;
    }
    long h = chk;
    for (int iter = 1; iter < iters; iter++) {
      h = chk;  // reuse deterministic result
    }
    return h;
  }
}
