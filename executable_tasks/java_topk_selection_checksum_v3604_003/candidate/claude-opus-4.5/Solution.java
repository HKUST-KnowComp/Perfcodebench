import java.util.*;

public class Solution {

  public static long run(int[] scores, int k, int iters) {
    int n = scores.length;
    int[] idx = new int[n];
    int[] sc = new int[n];
    int[] topIdx = new int[k];
    int[] topSc = new int[k];
    long h = 0;

    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < n; i++) {
        idx[i] = i;
        sc[i] = scores[i];
      }

      // Quickselect to partition top k elements
      int lo = 0, hi = n - 1;
      while (lo < hi) {
        int pivotIdx = lo + ((hi - lo) >> 1);
        int pivotScore = sc[pivotIdx];
        int pivotIndex = idx[pivotIdx];
        
        // Move pivot to end
        sc[pivotIdx] = sc[hi];
        idx[pivotIdx] = idx[hi];
        sc[hi] = pivotScore;
        idx[hi] = pivotIndex;
        
        int store = lo;
        for (int i = lo; i < hi; i++) {
          // Compare: higher score wins, tie-break by lower index
          if (sc[i] > pivotScore || (sc[i] == pivotScore && idx[i] < pivotIndex)) {
            int ts = sc[store];
            int ti = idx[store];
            sc[store] = sc[i];
            idx[store] = idx[i];
            sc[i] = ts;
            idx[i] = ti;
            store++;
          }
        }
        
        // Move pivot to final position
        int ts = sc[store];
        int ti = idx[store];
        sc[store] = sc[hi];
        idx[store] = idx[hi];
        sc[hi] = ts;
        idx[hi] = ti;
        
        if (store == k - 1) {
          break;
        } else if (store < k - 1) {
          lo = store + 1;
        } else {
          hi = store - 1;
        }
      }

      // Copy top k to separate arrays for sorting
      for (int i = 0; i < k; i++) {
        topSc[i] = sc[i];
        topIdx[i] = idx[i];
      }

      // Insertion sort the top k (small k, cache-friendly)
      for (int i = 1; i < k; i++) {
        int s = topSc[i];
        int x = topIdx[i];
        int j = i - 1;
        while (j >= 0 && (topSc[j] < s || (topSc[j] == s && topIdx[j] > x))) {
          topSc[j + 1] = topSc[j];
          topIdx[j + 1] = topIdx[j];
          j--;
        }
        topSc[j + 1] = s;
        topIdx[j + 1] = x;
      }

      // Compute hash
      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        h ^= (((long) topSc[i]) << 32) ^ (topIdx[i] & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}
