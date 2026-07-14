import java.util.*;

public class Solution {

  public static long run(int[] scores, int k, int iters) {
    int n = scores.length;
    if (k <= 0 || n == 0) {
      long h = 1469598103934665603L;
      for (int iter = 0; iter < iters; iter++) {
        h = 1469598103934665603L;
      }
      return h;
    }
    if (k > n) k = n;

    // Heap arrays: store (score, index) encoded for min-heap by score desc, index asc
    // Min-heap property: smallest score at root; on equal score, largest index at root
    int[] heapScore = new int[k];
    int[] heapIndex = new int[k];

    // Result arrays for final sorted top-k
    int[] resScore = new int[k];
    int[] resIndex = new int[k];

    long h = 0;
    for (int iter = 0; iter < iters; iter++) {
      int heapSize = 0;

      // Build min-heap of top-k
      for (int i = 0; i < n; i++) {
        int sc = scores[i];
        if (heapSize < k) {
          // Insert into heap
          int pos = heapSize++;
          heapScore[pos] = sc;
          heapIndex[pos] = i;
          // Sift up
          while (pos > 0) {
            int parent = (pos - 1) >> 1;
            // Min-heap: parent should be <= child
            // Compare: smaller score is "smaller"; on tie, larger index is "smaller"
            if (heapScore[parent] < sc || (heapScore[parent] == sc && heapIndex[parent] > i)) {
              break;
            }
            heapScore[pos] = heapScore[parent];
            heapIndex[pos] = heapIndex[parent];
            pos = parent;
          }
          heapScore[pos] = sc;
          heapIndex[pos] = i;
        } else {
          // Check if current element should replace root
          int rootSc = heapScore[0];
          int rootIdx = heapIndex[0];
          // Current is "better" if score > rootSc, or score == rootSc && index < rootIdx
          if (sc > rootSc || (sc == rootSc && i < rootIdx)) {
            // Replace root and sift down
            int pos = 0;
            int nsc = sc;
            int nidx = i;
            while (true) {
              int left = (pos << 1) + 1;
              if (left >= k) break;
              int right = left + 1;
              int smallest = left;
              int ssc = heapScore[left];
              int sidx = heapIndex[left];
              if (right < k) {
                int rsc = heapScore[right];
                int ridx = heapIndex[right];
                // Pick smaller child
                if (rsc < ssc || (rsc == ssc && ridx > sidx)) {
                  smallest = right;
                  ssc = rsc;
                  sidx = ridx;
                }
              }
              // If child is smaller than current, swap
              if (ssc < nsc || (ssc == nsc && sidx > nidx)) {
                heapScore[pos] = ssc;
                heapIndex[pos] = sidx;
                pos = smallest;
              } else {
                break;
              }
            }
            heapScore[pos] = nsc;
            heapIndex[pos] = nidx;
          }
        }
      }

      // Extract heap into result arrays
      for (int i = 0; i < k; i++) {
        resScore[i] = heapScore[i];
        resIndex[i] = heapIndex[i];
      }

      // Sort result by score desc, index asc using insertion sort (fast for small k)
      for (int i = 1; i < k; i++) {
        int sc = resScore[i];
        int idx = resIndex[i];
        int j = i - 1;
        while (j >= 0 && (resScore[j] < sc || (resScore[j] == sc && resIndex[j] > idx))) {
          resScore[j + 1] = resScore[j];
          resIndex[j + 1] = resIndex[j];
          j--;
        }
        resScore[j + 1] = sc;
        resIndex[j + 1] = idx;
      }

      // Compute checksum
      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        h ^= (((long) resScore[i]) << 32) ^ (resIndex[i] & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}
