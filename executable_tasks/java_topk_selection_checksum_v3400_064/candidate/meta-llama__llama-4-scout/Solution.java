java
import java.util.*;

public class Solution {
  static class Pair {
    int score;
    int index;
    Pair(int score, int index) { this.score = score; this.index = index; }
  }

  public static long run(int[] scores, int k, int iters) {
    PriorityQueue<Pair> pq = new PriorityQueue<>((a, b) -> {
      if (a.score != b.score) return b.score - a.score;
      return a.index - b.index;
    });
    long h = 0;
    for (int iter = 0; iter < iters; iter++) {
      pq.clear();
      for (int score : scores) {
        pq.offer(new Pair(score, scores.length - 1));
        if (pq.size() > k) pq.poll();
      }
      h = 1469598103934665603L;
      Object[] array = pq.toArray();
      for (int i = 0; i < k; i++) {
        Pair p = (Pair) array[k - 1 - i];
        h ^= (((long) p.score) << 32) ^ (p.index & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}