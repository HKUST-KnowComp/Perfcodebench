import java.util.*;

public class Solution {
  static class Pair {
    int score;
    int index;
    Pair(int score, int index) { this.score = score; this.index = index; }
  }

  static boolean better(Pair a, Pair b) {
    if (a.score != b.score) return a.score > b.score;
    return a.index < b.index;
  }

  public static long run(int[] scores, int k, int iters) {
    long h = 0;
    for (int iter = 0; iter < iters; iter++) {
      PriorityQueue<Pair> pq = new PriorityQueue<>((a, b) -> {
        if (a.score != b.score) return Integer.compare(a.score, b.score);
        return Integer.compare(b.index, a.index);
      });
      for (int i = 0; i < scores.length; i++) {
        Pair cur = new Pair(scores[i], i);
        if (pq.size() < k) {
          pq.add(cur);
        } else if (better(cur, pq.peek())) {
          pq.poll();
          pq.add(cur);
        }
      }
      Pair[] out = pq.toArray(new Pair[0]);
      Arrays.sort(out, (a, b) -> {
        if (a.score != b.score) return Integer.compare(b.score, a.score);
        return Integer.compare(a.index, b.index);
      });
      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        h ^= (((long) out[i].score) << 32) ^ (out[i].index & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}
