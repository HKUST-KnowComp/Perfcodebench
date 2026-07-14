import java.util.*;

public class Solution {
  static class Pair {
    int score;
    int index;
    Pair(int score, int index) { this.score = score; this.index = index; }
  }

  private static final Comparator<Pair> PAIR_COMPARATOR = (a, b) -> {
    if (a.score != b.score) return Integer.compare(b.score, a.score);
    return Integer.compare(a.index, b.index);
  };

  public static long run(int[] scores, int k, int iters) {
    final int n = scores.length;
    // Preallocate all Pair objects once to avoid per-iteration allocation overhead
    Pair[] pairs = new Pair[n];
    for (int i = 0; i < n; i++) {
      pairs[i] = new Pair(0, 0);
    }
    // Preinitialize min-heap once to track top-k elements, with worst candidate always at the head
    PriorityQueue<Pair> heap = new PriorityQueue<>(k, (a, b) -> {
      if (a.score != b.score) return Integer.compare(a.score, b.score);
      return Integer.compare(b.index, a.index);
    });
    long h = 0;
    for (int iter = 0; iter < iters; iter++) {
      heap.clear();
      // Update preallocated pairs with current values, no new objects created
      for (int i = 0; i < n; i++) {
        Pair p = pairs[i];
        p.score = scores[i];
        p.index = i;
        // Maintain heap size k, only keep the top performing elements
        if (heap.size() < k) {
          heap.add(p);
        } else {
          Pair currentWorst = heap.peek();
          if (PAIR_COMPARATOR.compare(p, currentWorst) < 0) {
            heap.poll();
            heap.add(p);
          }
        }
      }
      // Sort top-k elements to match original ordering for correct checksum calculation
      List<Pair> topK = new ArrayList<>(heap);
      Collections.sort(topK, PAIR_COMPARATOR);
      // Compute checksum exactly as original implementation to preserve correctness
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