import java.util.*;

public class Solution {
  static class Pair {
    int score;
    int index;
    Pair(int score, int index) { this.score = score; this.index = index; }
  }

  public static long run(int[] scores, int k, int iters) {
    Pair[] pairs = new Pair[scores.length];
    long h = 0;
    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < scores.length; i++) pairs[i] = new Pair(scores[i], i);
      Arrays.sort(pairs, (a, b) -> {
        if (a.score != b.score) return Integer.compare(b.score, a.score);
        return Integer.compare(a.index, b.index);
      });
      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        h ^= (((long) pairs[i].score) << 32) ^ (pairs[i].index & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}
