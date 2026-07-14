import java.util.*;

public class Solution {
  static class Pair {
    int score;
    int index;
    Pair(int score, int index) { this.score = score; this.index = index; }
  }

  public static long run(int[] scores, int k, int iters) {
    int n = scores.length;
    // Preallocate all Pair objects once to eliminate GC churn
    Pair[] pairs = new Pair[n];
    for (int i = 0; i < n; i++) {
      pairs[i] = new Pair(0, 0);
    }
    // Heap comparator: min-heap where the worst element of current top-k is at the head
    Comparator<Pair> heapComp = (a, b) -> {
      if (a.score != b.score) return Integer.compare(a.score, b.score);
      return Integer.compare(b.index, a.index);
    };
    // Reusable priority queue created once
    PriorityQueue<Pair> topKHeap = new PriorityQueue<>(k + 1, heapComp);
    // Sort comparator to match original baseline's ordering
    Comparator<Pair> sortComp = (a, b) -> {
      if (a.score != b.score) return Integer.compare(b.score, a.score);
      return Integer.compare(a.index, b.index);
    };

    long h = 0;
    for (int iter = 0; iter < iters; iter++) {
      // Update existing pairs instead of allocating new ones
      for (int i = 0; i < n; i++) {
        pairs[i].score = scores[i];
        pairs[i].index = i;
      }
      topKHeap.clear();
      // Process all elements to select top k
      for (Pair p : pairs) {
        if (topKHeap.size() < k) {
          topKHeap.add(p);
        } else {
          Pair worst = topKHeap.peek();
          // Check if current element is better than the worst in the top k
          if (worst.score < p.score || (worst.score == p.score && worst.index > p.index)) {
            topKHeap.poll();
            topKHeap.add(p);
          }
        }
      }
      // Extract and sort top k to match baseline order for correct checksum
      Pair[] topK = new Pair[k];
      int idx = 0;
      while (!topKHeap.isEmpty()) {
        topK[idx++] = topKHeap.poll();
      }
      Arrays.sort(topK, sortComp);
      // Calculate checksum exactly as baseline
      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        h ^= (((long) topK[i].score) << 32) ^ (topK[i].index & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}