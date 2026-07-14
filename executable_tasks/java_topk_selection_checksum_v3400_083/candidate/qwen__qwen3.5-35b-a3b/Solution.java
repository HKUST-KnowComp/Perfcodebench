import java.util.*;

public class Solution {
  public static long run(int[] scores, int k, int iters) {
    long h = 0;
    int n = scores.length;
    for (int iter = 0; iter < iters; iter++) {
      PriorityQueue<Integer> pq = new PriorityQueue<>((a, b) -> {
        if (scores[a] != scores[b]) {
          return Integer.compare(scores[a], scores[b]);
        } else {
          return Integer.compare(b, a);
        }
      });
      for (int i = 0; i < n; i++) {
        pq.offer(i);
        if (pq.size() > k) {
          pq.poll();
        }
      }
      List<Integer> topK = new ArrayList<>(pq);
      topK.sort((a, b) -> {
        if (scores[a] != scores[b]) {
          return Integer.compare(scores[b], scores[a]);
        } else {
          return Integer.compare(a, b);
        }
      });
      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        int idx = topK.get(i);
        h ^= (((long) scores[idx]) << 32) ^ (idx & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}