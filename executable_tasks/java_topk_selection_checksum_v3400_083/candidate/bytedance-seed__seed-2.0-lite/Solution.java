import java.util.*;

public class Solution {
  static class Pair {
    int score;
    int index;
    Pair(int score, int index) { this.score = score; this.index = index; }
  }

  private static final Comparator<Pair> FULL_ORDER_COMPARATOR = (a, b) -> {
    if (a.score != b.score) return Integer.compare(b.score, a.score);
    return Integer.compare(a.index, b.index);
  };

  private static final Comparator<Pair> MIN_HEAP_COMPARATOR = (a, b) -> {
    if (a.score != b.score) return Integer.compare(a.score, b.score);
    return Integer.compare(b.index, a.index);
  };

  public static long run(int[] scores, int k, int iters) {
    final int n = scores.length;
    Pair[] pairs = new Pair[n];
    for (int i = 0; i < n; i++) {
      pairs[i] = new Pair(scores[i], i);
    }

    PriorityQueue<Pair> heap = new PriorityQueue<>(k, MIN_HEAP_COMPARATOR);
    long h = 0;

    for (int iter = 0; iter < iters; iter++) {
      heap.clear();
      for (Pair p : pairs) {
        if (heap.size() < k) {
          heap.add(p);
        } else {
          Pair currentMin = heap.peek();
          if (FULL_ORDER_COMPARATOR.compare(p, currentMin) < 0) {
            heap.poll();
            heap.add(p);
          }
        }
      }

      Pair[] topK = new Pair[k];
      heap.toArray(topK);
      Arrays.sort(topK, FULL_ORDER_COMPARATOR);

      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        h ^= (((long) topK[i].score) << 32) ^ (topK[i].index & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}