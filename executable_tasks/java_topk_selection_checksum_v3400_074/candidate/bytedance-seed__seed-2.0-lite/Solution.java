import java.util.*;

public class Solution {
  static class Pair {
    int score;
    int index;
    Pair(int score, int index) { this.score = score; this.index = index; }
  }

  public static long run(int[] scores, int k, int iters) {
    int n = scores.length;
    Pair[] pairs = new Pair[n];
    // Preallocate all pairs once to eliminate GC overhead of repeated object creation
    for (int i = 0; i < n; i++) {
      pairs[i] = new Pair(0, 0);
    }
    
    final Comparator<Pair> TOPK_ORDER = (a, b) -> {
      if (a.score != b.score) return Integer.compare(b.score, a.score);
      return Integer.compare(a.index, b.index);
    };

    long h = 0;
    for (int iter = 0; iter < iters; iter++) {
      // Update existing pair values instead of creating new objects
      for (int i = 0; i < n; i++) {
        pairs[i].score = scores[i];
        pairs[i].index = i;
      }

      // Min-heap to track only top-k elements, O(n log k) vs original O(n log n) full sort
      PriorityQueue<Pair> heap = new PriorityQueue<>(k + 1, TOPK_ORDER);
      for (Pair p : pairs) {
        heap.add(p);
        if (heap.size() > k) heap.poll();
      }

      // Sort top-k elements to match original ordering for identical checksum
      List<Pair> topK = new ArrayList<>(heap);
      Collections.sort(topK, TOPK_ORDER);

      // Exact same hash computation as original to preserve correctness
      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        Pair p = topK.get(i);
        h ^= (((long) p.score) << 32) ^ (p.index & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}