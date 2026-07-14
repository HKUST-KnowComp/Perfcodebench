import java.util.*;

public class Solution {
  static class Pair {
    int score;
    int index;
    Pair(int score, int index) { this.score = score; this.index = index; }
  }

  private static final Comparator<Pair> FULL_SORT_COMPARATOR = (a, b) -> {
    if (a.score != b.score) return Integer.compare(b.score, a.score);
    return Integer.compare(a.index, b.index);
  };

  private static final Comparator<Pair> MIN_HEAP_COMPARATOR = (a, b) -> {
    if (a.score != b.score) return Integer.compare(a.score, b.score);
    return Integer.compare(b.index, a.index);
  };

  public static long run(int[] scores, int k, int iters) {
    Pair[] pairs = new Pair[scores.length];
    for (int i = 0; i < scores.length; i++) {
      pairs[i] = new Pair(scores[i], i);
    }
    long h = 0;
    for (int iter = 0; iter < iters; iter++) {
      PriorityQueue<Pair> heap = new PriorityQueue<>(k, MIN_HEAP_COMPARATOR);
      for (Pair p : pairs) {
        heap.add(p);
        if (heap.size() > k) {
          heap.poll();
        }
      }
      Pair[] topK = new Pair[k];
      int idx = 0;
      while (!heap.isEmpty()) {
        topK[idx++] = heap.poll();
      }
      Arrays.sort(topK, FULL_SORT_COMPARATOR);
      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        h ^= (((long) topK[i].score) << 32) ^ (topK[i].index & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}