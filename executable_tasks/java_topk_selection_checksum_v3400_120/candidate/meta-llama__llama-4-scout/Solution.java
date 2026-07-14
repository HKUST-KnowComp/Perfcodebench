import java.util.*;

public class Solution {
  public static long run(int[] scores, int k, int iters) {
    PriorityQueue<Pair> pq = new PriorityQueue<>((a, b) -> {
      if (a.score != b.score) return Integer.compare(b.score, a.score);
      return Integer.compare(a.index, b.index);
    });
    long h = 0;
    for (int iter = 0; iter < iters; iter++) {
      pq.clear();
      for (int i = 0; i < scores.length; i++) {
        pq.offer(new Pair(scores[i], i));
        if (pq.size() > k) pq.poll();
      }
      h = 1469598103934665603L;
      Object[] arr = pq.toArray();
      for (int i = 0; i < k; i++) {
        Pair pair = (Pair) arr[i];
        h ^= (((long) pair.score) << 32) ^ (pair.index & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
  static class Pair {
    int score;
    int index;
    Pair(int score, int index) { this.score = score; this.index = index; }
  }
}