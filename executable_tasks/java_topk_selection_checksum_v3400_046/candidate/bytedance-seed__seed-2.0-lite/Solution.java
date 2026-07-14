import java.util.*;

public class Solution {
  static class Pair {
    int score;
    int index;
    Pair(int score, int index) { this.score = score; this.index = index; }
  }

  private static final Comparator<Pair> GLOBAL_SORT_ORDER = (a, b) -> {
    if (a.score != b.score) return Integer.compare(b.score, a.score);
    return Integer.compare(a.index, b.index);
  };

  private static final Comparator<Pair> MIN_HEAP_ORDER = (a, b) -> {
    if (a.score != b.score) return Integer.compare(a.score, b.score);
    return Integer.compare(b.index, a.index);
  };

  public static long run(int[] scores, int k, int iters) {
    final int n = scores.length;
    Pair[] allPairs = new Pair[n];
    for (int i = 0; i < n; i++) {
      allPairs[i] = new Pair(scores[i], i);
    }

    PriorityQueue<Pair> topKHeap = new PriorityQueue<>(k, MIN_HEAP_ORDER);
    Pair[] topK = new Pair[Math.min(k, n)];
    long h = 0;

    for (int iter = 0; iter < iters; iter++) {
      topKHeap.clear();
      for (Pair p : allPairs) {
        if (topKHeap.size() < k) {
          topKHeap.add(p);
        } else {
          Pair currentWorst = topKHeap.peek();
          if (GLOBAL_SORT_ORDER.compare(p, currentWorst) < 0) {
            topKHeap.poll();
            topKHeap.add(p);
          }
        }
      }
      topKHeap.toArray(topK);
      Arrays.sort(topK, GLOBAL_SORT_ORDER);
      h = 1469598103934665603L;
      for (int i = 0; i < topK.length; i++) {
        h ^= (((long) topK[i].score) << 32) ^ (topK[i].index & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}