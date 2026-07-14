import java.util.*;

public class Solution {
  static class Pair {
    int score;
    int index;
    Pair(int score, int index) { this.score = score; this.index = index; }
  }

  public static long run(int[] scores, int k, int iters) {
    // Pre-allocate pairs once to avoid repeated GC of new objects per iteration
    Pair[] pairs = new Pair[scores.length];
    for (int i = 0; i < scores.length; i++) {
      pairs[i] = new Pair(scores[i], i);
    }
    long h = 0;
    // Min-heap comparator: keeps worst candidate of current top-k at root
    Comparator<Pair> heapComp = (a, b) -> {
      if (a.score != b.score) return Integer.compare(a.score, b.score);
      return Integer.compare(a.index, b.index);
    };
    // Original sort comparator to match the exact top-k ordering from the baseline
    Comparator<Pair> sortComp = (a, b) -> {
      if (a.score != b.score) return Integer.compare(b.score, a.score);
      return Integer.compare(a.index, b.index);
    };

    for (int iter = 0; iter < iters; iter++) {
      PriorityQueue<Pair> heap = new PriorityQueue<>(k, heapComp);
      for (Pair p : pairs) {
        heap.add(p);
        if (heap.size() > k) heap.poll();
      }
      // Extract and sort top-k to match baseline order for identical checksum
      Pair[] topK = new Pair[k];
      for (int i = 0; i < k; i++) {
        topK[i] = heap.poll();
      }
      Arrays.sort(topK, sortComp);
      // Exact same checksum calculation as original
      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        h ^= (((long) topK[i].score) << 32) ^ (topK[i].index & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}