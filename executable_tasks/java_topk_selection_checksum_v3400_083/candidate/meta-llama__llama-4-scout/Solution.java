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
      for (int i = 0; i < scores.length; i++) {
        pq.offer(new Pair(scores[i], i));
        if (pq.size() > k) pq.poll();
      }
      h = 1469598103934665603L;
      Object[] array = pq.toArray();
      Arrays.sort(array, (a, b) -> {
        Pair p1 = (Pair) a;
        Pair p2 = (Pair) b;
        if (p1.score != p2.score) return Integer.compare(p2.score, p1.score);
        return Integer.compare(p1.index, p2.index);
      });
      for (int i = 0; i < k; i++) {
        Pair p = (Pair) array[i];
        h ^= (((long) p.score) << 32) ^ (p.index & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}