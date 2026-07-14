import java.util.*;

public class Solution {
  static class Pair {
    int score;
    int index;
    Pair(int score, int index) { this.score = score; this.index = index; }
  }

  public static long run(int[] scores, int k, int iters) {
    long h = 0;
    for (int iter = 0; iter < iters; iter++) {
      PriorityQueue<Pair> pq = new PriorityQueue<>(k, (a, b) -> {
        if (a.score != b.score) {
          return Integer.compare(a.score, b.score);
        } else {
          return Integer.compare(b.index, a.index);
        }
      });

      for (int i = 0; i < scores.length; i++) {
        Pair p = new Pair(scores[i], i);
        if (pq.size() < k) {
          pq.add(p);
        } else {
          Pair head = pq.peek();
          if (p.score > head.score || (p.score == head.score && p.index < head.index)) {
            pq.poll();
            pq.add(p);
          }
        }
      }

      List<Pair> topK = new ArrayList<>(pq);
      Collections.sort(topK, (a, b) -> {
        if (a.score != b.score) {
          return Integer.compare(b.score, a.score);
        } else {
          return Integer.compare(a.index, b.index);
        }
      });

      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        h ^= (((long) topK.get(i).score) << 32) ^ (topK.get(i).index & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}