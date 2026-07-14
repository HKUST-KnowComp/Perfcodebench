import java.util.*;

public class Solution {
  static class Pair {
    int score;
    int index;
    Pair(int score, int index) { this.score = score; this.index = index; }
  }

  public static long run(int[] scores, int k, int iters) {
    // Preallocate all pairs once to avoid repeated object allocation/GC churn
    Pair[] pairs = new Pair[scores.length];
    for (int i = 0; i < scores.length; i++) pairs[i] = new Pair(scores[i], i);
    
    long h = 0;
    // Original sort order comparator to preserve exact top-k ordering for checksum
    final Comparator<Pair> originalOrderComparator = (a, b) -> {
      if (a.score != b.score) return Integer.compare(b.score, a.score);
      return Integer.compare(a.index, b.index);
    };
    // Min-heap comparator: worst of current top-k sits at the heap head for quick replacement
    final Comparator<Pair> heapComparator = (a, b) -> {
      if (a.score != b.score) return Integer.compare(a.score, b.score);
      return Integer.compare(b.index, a.index);
    };

    for (int iter = 0; iter < iters; iter++) {
      PriorityQueue<Pair> topKHeap = new PriorityQueue<>(k, heapComparator);
      for (Pair p : pairs) {
        if (topKHeap.size() < k) {
          topKHeap.add(p);
        } else {
          Pair currentWorst = topKHeap.peek();
          // Replace worst in heap if current element is better (comes first in original order)
          if (originalOrderComparator.compare(p, currentWorst) < 0) {
            topKHeap.poll();
            topKHeap.add(p);
          }
        }
      }
      // Sort top-k to match original ordering for identical checksum calculation
      Pair[] topK = new Pair[k];
      topKHeap.toArray(topK);
      Arrays.sort(topK, originalOrderComparator);
      
      // Reuse exact same checksum logic from baseline to guarantee correctness
      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        h ^= (((long) topK[i].score) << 32) ^ (topK[i].index & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}