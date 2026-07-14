import java.util.*;

public class Solution {

  public static long run(int[] scores, int k, int iters) {
    int n = scores.length;
    long h = 0;
    // Heap stores encoded values: high 32 bits = score, low 32 bits = index
    // For min-heap by (score desc, index asc), we use negative score in high bits
    long[] heap = new long[k];
    long[] topk = new long[k];

    for (int iter = 0; iter < iters; iter++) {
      int heapSize = 0;

      for (int i = 0; i < n; i++) {
        int score = scores[i];
        // Encode: negate score for min-heap behavior (we want max scores)
        // Use index directly (smaller index wins ties, so smaller is better)
        long encoded = ((long)(-score) << 32) | (i & 0xffffffffL);

        if (heapSize < k) {
          // Add to heap
          heap[heapSize] = encoded;
          // Sift up
          int c = heapSize;
          while (c > 0) {
            int p = (c - 1) >> 1;
            if (heap[c] < heap[p]) {
              long tmp = heap[c];
              heap[c] = heap[p];
              heap[p] = tmp;
              c = p;
            } else {
              break;
            }
          }
          heapSize++;
        } else if (encoded < heap[0]) {
          // New element is better than worst in heap (smaller encoded = higher score or same score with smaller index)
          heap[0] = encoded;
          // Sift down
          int p = 0;
          while (true) {
            int left = (p << 1) + 1;
            if (left >= k) break;
            int right = left + 1;
            int smallest = p;
            if (heap[left] < heap[smallest]) smallest = left;
            if (right < k && heap[right] < heap[smallest]) smallest = right;
            if (smallest == p) break;
            long tmp = heap[p];
            heap[p] = heap[smallest];
            heap[smallest] = tmp;
            p = smallest;
          }
        }
      }

      // Extract in sorted order (best first = smallest encoded first)
      for (int i = 0; i < k; i++) {
        topk[i] = heap[0];
        heap[0] = heap[k - 1 - i];
        int size = k - 1 - i;
        // Sift down
        int p = 0;
        while (true) {
          int left = (p << 1) + 1;
          if (left >= size) break;
          int right = left + 1;
          int smallest = p;
          if (heap[left] < heap[smallest]) smallest = left;
          if (right < size && heap[right] < heap[smallest]) smallest = right;
          if (smallest == p) break;
          long tmp = heap[p];
          heap[p] = heap[smallest];
          heap[smallest] = tmp;
          p = smallest;
        }
      }

      // Compute checksum
      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        long enc = topk[i];
        int score = (int)(-(enc >> 32));
        int index = (int)enc;
        h ^= (((long) score) << 32) ^ (index & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}
