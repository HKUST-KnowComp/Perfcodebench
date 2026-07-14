java
import java.util.*;

public class Solution {
  static class Pair {
    int score;
    int index;
    Pair(int score, int index) { this.score = score; this.index = index; }
  }

  public static long run(int[] scores, int k, int iters) {
    PriorityQueue<Pair> queue = new PriorityQueue<>((a, b) -> {
      if (a.score != b.score) return b.score - a.score;
      return a.index - b.index;
    });
    long h = 0;
    for (int iter = 0; iter < iters; iter++) {
      queue.clear();
      for (int score : scores) {
        queue.add(new Pair(score, scores.length - 1));
        if (queue.size() > k) queue.poll();
      }
      h = 1469598103934665603L;
      Object[] array = queue.toArray();
      for (int i = 0; i < k; i++) {
        Pair pair = (Pair) array[k - i - 1];
        h ^= (((long) pair.score) << 32) ^ (pair.index & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}