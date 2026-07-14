import java.util.*;

public class Solution {
  static class Pair {
    int score;
    int index;
    Pair(int score, int index) { this.score = score; this.index = index; }
  }

  public static long run(int[] scores, int k, int iters) {
    int n = scores.length;
    if (n == 0 || k <= 0) return 0;
    final int effectiveK = Math.min(k, n);
    
    // Precreate all Pair objects once, avoid redundant allocation
    Pair[] pairs = new Pair[n];
    for (int i = 0; i < n; i++) {
      pairs[i] = new Pair(scores[i], i);
    }
    
    // Min-heap to track top-k elements: worst element (lowest score, highest index) at head
    PriorityQueue<Pair> heap = new PriorityQueue<>(effectiveK + 1, (a, b) -> {
      if (a.score != b.score) return Integer.compare(a.score, b.score);
      return Integer.compare(b.index, a.index);
    });
    
    // Preallocate array to hold top-k elements for final sorting
    Pair[] topK = new Pair[effectiveK];
    long h = 0;
    
    for (int iter = 0; iter < iters; iter++) {
      heap.clear();
      
      // Process all elements to select top-k
      for (Pair p : pairs) {
        if (heap.size() < effectiveK) {
          heap.add(p);
        } else {
          Pair head = heap.peek();
          boolean isBetter = false;
          if (p.score > head.score) {
            isBetter = true;
          } else if (p.score == head.score && p.index < head.index) {
            isBetter = true;
          }
          if (isBetter) {
            heap.poll();
            heap.add(p);
          }
        }
      }
      
      // Sort top-k elements to match original baseline ordering for correct checksum
      heap.toArray(topK);
      Arrays.sort(topK, (a, b) -> {
        if (a.score != b.score) return Integer.compare(b.score, a.score);
        return Integer.compare(a.index, b.index);
      });
      
      // Compute identical checksum as baseline
      h = 1469598103934665603L;
      for (int i = 0; i < effectiveK; i++) {
        h ^= (((long) topK[i].score) << 32) ^ (topK[i].index & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}