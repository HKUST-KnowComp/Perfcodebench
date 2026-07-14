import java.util.*;

public class Solution {
  static class Pair {
    int score;
    int index;
    Pair(int score, int index) { this.score = score; this.index = index; }
  }

  private static final Comparator<Pair> MIN_HEAP_COMPARATOR = (a, b) -> {
    if (a.score != b.score) return Integer.compare(a.score, b.score);
    return Integer.compare(a.index, b.index);
  };

  private static final Comparator<Pair> SORT_COMPARATOR = (a, b) -> {
    if (a.score != b.score) return Integer.compare(b.score, a.score);
    return Integer.compare(a.index, b.index);
  };

  public static long run(int[] scores, int k, int iters) {
    int n = scores.length;
    // Preallocate all reusable structures once to avoid GC overhead
    Pair[] pairs = new Pair[n];
    for (int i = 0; i < n; i++) {
      pairs[i] = new Pair(scores[i], i);
    }
    Pair[] topK = new Pair[k];
    PriorityQueue<Pair> heap = new PriorityQueue<>(k, MIN_HEAP_COMPARATOR);
    long h = 0;

    for (int iter = 0; iter < iters; iter++) {
      heap.clear();
      // Build min-heap of top-k elements
      for (Pair p : pairs) {
        if (heap.size() < k) {
          heap.offer(p);
        } else {
          if (SORT_COMPARATOR.compare(p, heap.peek()) < 0) {
            heap.poll();
            heap.offer(p);
          }
        }
      }
      // Sort top-k to match original ordering for correct checksum
      heap.toArray(topK);
      Arrays.sort(topK, SORT_COMPARATOR);
      // Exact same checksum calculation as baseline
      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        h ^= (((long) topK[i].score) << 32) ^ (topK[i].index & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}