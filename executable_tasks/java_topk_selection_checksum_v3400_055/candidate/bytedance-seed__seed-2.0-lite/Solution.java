import java.util.*;

public class Solution {
  static class Pair {
    int score;
    int index;
    Pair(int score, int index) { this.score = score; this.index = index; }
  }

  public static long run(int[] scores, int k, int iters) {
    // Preallocate pairs once for all iterations since input scores are fixed
    Pair[] pairs = new Pair[scores.length];
    for (int i = 0; i < scores.length; i++) pairs[i] = new Pair(scores[i], i);
    
    long h = 0;
    // Comparator to sort top-k elements, matches original sort order exactly
    final Comparator<Pair> FULL_SORT_COMPARATOR = (a, b) -> {
      if (a.score != b.score) return Integer.compare(b.score, a.score);
      return Integer.compare(a.index, b.index);
    };
    // Min-heap comparator: evicts the worst element (lowest score, highest index for ties) first
    final Comparator<Pair> HEAP_COMPARATOR = (a, b) -> {
      if (a.score != b.score) return Integer.compare(a.score, b.score);
      return Integer.compare(b.index, a.index);
    };

    for (int iter = 0; iter < iters; iter++) {
      PriorityQueue<Pair> topKHeap = new PriorityQueue<>(k+1, HEAP_COMPARATOR);
      for (Pair p : pairs) {
        topKHeap.add(p);
        if (topKHeap.size() > k) topKHeap.poll();
      }
      // Convert heap to sorted array to match original order for checksum
      Pair[] topK = new Pair[topKHeap.size()];
      topKHeap.toArray(topK);
      Arrays.sort(topK, FULL_SORT_COMPARATOR);
      
      // Exact same checksum calculation as original implementation
      h = 1469598103934665603L;
      for (int i = 0; i < topK.length; i++) {
        h ^= (((long) topK[i].score) << 32) ^ (topK[i].index & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}