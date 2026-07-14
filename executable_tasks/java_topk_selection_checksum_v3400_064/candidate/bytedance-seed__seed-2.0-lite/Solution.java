import java.util.*;

public class Solution {
  static class Pair {
    int score;
    int index;
    Pair(int score, int index) { this.score = score; this.index = index; }
  }

  public static long run(int[] scores, int k, int iters) {
    // Pre-create all pairs once to avoid repeated allocation
    Pair[] pairs = new Pair[scores.length];
    for (int i = 0; i < scores.length; i++) {
      pairs[i] = new Pair(scores[i], i);
    }

    // Heap comparator: evict worst element first (lowest score, then highest index)
    Comparator<Pair> heapComp = (a, b) -> {
      if (a.score != b.score) return Integer.compare(a.score, b.score);
      return Integer.compare(b.index, a.index);
    };
    // Reusable min-heap, pre-sized to k
    PriorityQueue<Pair> heap = new PriorityQueue<>(k, heapComp);
    // Reusable top-k array to avoid per-iteration allocation
    Pair[] topK = new Pair[k];
    // Sort comparator matching baseline's full sort order to preserve checksum correctness
    Comparator<Pair> sortComp = (a, b) -> {
      if (a.score != b.score) return Integer.compare(b.score, a.score);
      return Integer.compare(a.index, b.index);
    };

    long h = 0;
    for (int iter = 0; iter < iters; iter++) {
      heap.clear();
      for (Pair p : pairs) {
        heap.add(p);
        if (heap.size() > k) heap.poll();
      }
      // Extract elements from heap into reusable array
      int idx = 0;
      while (!heap.isEmpty()) topK[idx++] = heap.poll();
      // Sort to match baseline's ordered top-k sequence
      Arrays.sort(topK, sortComp);
      // Compute identical checksum as baseline
      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        h ^= (((long) topK[i].score) << 32) ^ (topK[i].index & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}