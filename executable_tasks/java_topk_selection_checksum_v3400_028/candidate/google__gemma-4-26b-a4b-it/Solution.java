import java.util.*;

public class Solution {
  public static long run(int[] scores, int k, int iters) {
    int n = scores.length;
    int[] s = new int[n];
    int[] idx = new int[n];
    long h = 0;

    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < n; i++) {
        s[i] = scores[i];
        idx[i] = i;
      }

      // Quickselect to find the top k elements
      quickSelect(s, idx, 0, n - 1, k);

      // The quickselect leaves the top k elements in the first k positions,
      // but they are not necessarily sorted among themselves. 
      // The problem requires the checksum over the ORDERED winners.
      // We must sort only the top k elements.
      sortTopK(s, idx, k);

      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        h ^= (((long) s[i]) << 32) ^ (idx[i] & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }

  private static void quickSelect(int[] s, int[] idx, int left, int right, int k) {
    while (left < right) {
      int pivotIndex = partition(s, idx, left, right);
      if (pivotIndex == k) return;
      if (pivotIndex < k) {
        left = pivotIndex + 1;
      } else {
        right = pivotIndex - 1;
      }
    }
  }

  private static int partition(int[] s, int[] idx, int left, int right) {
    int mid = left + (right - left) / 2;
    // Median-of-three to avoid worst-case on sorted arrays
    if (compare(s[mid], idx[mid], s[left], idx[left]) < 0) swap(s, idx, left, mid);
    if (compare(s[right], idx[right], s[left], idx[left]) < 0) swap(s, idx, left, right);
    if (compare(s[right], idx[right], s[mid], idx[mid]) < 0) swap(s, idx, mid, right);
    
    int pivotS = s[mid];
    int pivotIdx = idx[mid];
    swap(s, idx, mid, right - 1);
    
    int i = left;
    int j = right - 1;
    while (true) {
      while (compare(s[++i], idx[i], pivotS, pivotIdx) < 0);
      while (compare(pivotS, pivotIdx, s[--j], idx[j]) < 0);
      if (i >= j) break;
      swap(s, idx, i, j);
    }
    swap(s, idx, i, right - 1);
    return i;
  }

  private static void sortTopK(int[] s, int[] idx, int k) {
    // Use a custom sort for the top k elements to match the required ordering logic
    // Ordering: score DESC, then index ASC
    // Since k is usually small, Dual-Pivot Quicksort or simple insertion sort is fine.
    // We'll use a simple quicksort for the top k range.
    quickSort(s, idx, 0, k - 1);
  }

  private static void quickSort(int[] s, int[] idx, int left, int right) {
    if (left >= right) return;
    int pivotIdx = partitionForSort(s, idx, left, right);
    quickSort(s, idx, left, pivotIdx);
    quickSort(s, idx, pivotIdx + 1, right);
  }

  private static int partitionForSort(int[] s, int[] idx, int left, int right) {
    int mid = left + (right - left) / 2;
    int pivotS = s[mid];
    int pivotIdx = idx[mid];
    int i = left - 1;
    int j = right + 1;
    while (true) {
      do { i++; } while (compare(s[i], idx[i], pivotS, pivotIdx) < 0);
      do { j--; } while (compare(s[j], idx[j], pivotS, pivotIdx) > 0);
      if (i >= j) return j;
      swap(s, idx, i, j);
    }
  }

  private static int compare(int s1, int i1, int s2, int i2) {
    if (s1 != s2) return Integer.compare(s2, s1); // Descending score
    return Integer.compare(i1, i2); // Ascending index
  }

  private static void swap(int[] s, int[] idx, int i, int j) {
    int tempS = s[i];
    s[i] = s[j];
    s[j] = tempS;
    int tempIdx = idx[i];
    idx[i] = idx[j];
    idx[j] = tempIdx;
  }
}