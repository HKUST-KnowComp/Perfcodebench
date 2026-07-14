import java.util.*;

public class Solution {
  static class Pair {
    int score;
    int index;
    Pair(int score, int index) { this.score = score; this.index = index; }
  }

  public static long run(int[] scores, int k, int iters) {
    final int n = scores.length;
    // Precreate immutable Pair objects once to avoid per-iteration allocation
    Pair[] pairs = new Pair[n];
    for (int i = 0; i < n; i++) {
      pairs[i] = new Pair(scores[i], i);
    }
    // Preallocate reusable top-k storage and min-heap to avoid repeated allocation
    Pair[] topK = new Pair[k];
    Comparator<Pair> heapComparator = (a, b) -> {
      if (a.score != b.score) return Integer.compare(a.score, b.score);
      return Integer.compare(b.index, a.index);
    };
    PriorityQueue<Pair> minHeap = new PriorityQueue<>(k + 1, heapComparator);
    
    long h = 0;
    for (int iter = 0; iter < iters; iter++) {
      minHeap.clear();
      // Process all elements to find top-k
      for (Pair p : pairs) {
        if (minHeap.size() < k) {
          minHeap.add(p);
        } else {
          Pair worst = minHeap.peek();
          boolean isBetter = false;
          if (p.score > worst.score) {
            isBetter = true;
          } else if (p.score == worst.score && p.index < worst.index) {
            isBetter = true;
          }
          if (isBetter) {
            minHeap.poll();
            minHeap.add(p);
          }
        }
      }
      // Extract elements from heap (worst to best) into correctly ordered topK array
      for (int i = k - 1; i >= 0; i--) {
        topK[i] = minHeap.poll();
      }
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