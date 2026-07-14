import java.util.*;

public class Solution {
  /**
   * Optimized implementation using primitive long arrays to avoid object allocation
   * and Quickselect to achieve O(N) average time complexity.
   * 
   * We pack the score and index into a single long to allow efficient sorting/partitioning.
   * Since we need descending score and ascending index:
   * - We negate the score (or use bit manipulation) to make the natural long order
   *   match our custom comparator requirements.
   * - However, to keep it simple and avoid sign issues with bitwise negation, 
   *   we pack as: [32 bits: inverted score] [32 bits: index].
   *   Wait, the requirement is: higher score first, then lower index.
   *   Let's use: [32 bits: (Integer.MAX_VALUE - score)] [32 bits: index].
   *   This way, a natural ascending sort on this long will give us:
   *   1. Smallest (MAX - score) -> Largest score.
   *   2. Smallest index.
   */

  public static long run(int[] scores, int k, int iters) {
    int n = scores.length;
    long[] packed = new long[n];
    long h = 0;

    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < n; i++) {
        // Pack: high 32 bits = (Integer.MAX_VALUE - score), low 32 bits = index
        // This allows us to use natural ascending order to satisfy the requirements.
        // We use (Integer.MAX_VALUE - score) to ensure higher scores come first.
        // We use index directly to ensure lower indices come first.
        long high = (long) (Integer.MAX_VALUE - scores[i]) << 32;
        long low = (long) i & 0xFFFFFFFFL;
        packed[i] = high | low;
      }

      // Use Quickselect to find the k-th smallest element in the packed array.
      // After quickselect, the first k elements will be the top-k (in some order).
      quickSelect(packed, 0, n - 1, k);

      // The problem requires the checksum over the ORDERED winners.
      // Quickselect doesn't guarantee order within the top-k, so we must sort the top-k.
      Arrays.sort(packed, 0, k);

      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        long p = packed[i];
        int score = Integer.MAX_VALUE - (int) (p >>> 32);
        int index = (int) (p & 0xFFFFFFFFL);
        h ^= (((long) score) << 32) ^ (index & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }

  private static void quickSelect(long[] arr, int left, int right, int k) {
    if (left >= right) return;
    
    int pivotIndex = partition(arr, left, right);
    if (pivotIndex == k) return;
    if (pivotIndex > k) {
      quickSelect(arr, left, pivotIndex - 1, k);
    } else {
      quickSelect(arr, pivotIndex + 1, right, k);
    }
  }

  private static int partition(long[] arr, int left, int right) {
    // Median-of-three to avoid worst-case O(N^2)
    int mid = left + (right - left) / 2;
    if (arr[mid] < arr[left]) swap(arr, left, mid);
    if (arr[right] < arr[left]) swap(arr, left, right);
    if (arr[right] < arr[mid]) swap(arr, mid, right);
    
    long pivot = arr[mid];
    swap(arr, mid, right - 1);
    int i = left;
    int j = right - 1;
    
    while (true) {
      while (arr[++i] < pivot);
      while (arr[--j] > pivot);
      if (i >= j) break;
      swap(arr, i, j);
    }
    swap(arr, i, right - 1);
    return i;
  }

  private static void swap(long[] arr, int i, int j) {
    long temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
  }
}