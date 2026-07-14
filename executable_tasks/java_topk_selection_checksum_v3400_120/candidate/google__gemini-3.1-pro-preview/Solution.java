import java.util.*;

public class Solution {
  public static long run(int[] scores, int k, int iters) {
    int n = scores.length;
    // Pack score and index into a single long for efficient sorting.
    // High 32 bits: score (signed int).
    // Low 32 bits: (Integer.MAX_VALUE - index) to ensure ascending index order when sorting descending.
    long[] packed = new long[n];
    long h = 0;

    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < n; i++) {
        packed[i] = ((long) scores[i] << 32) | (long) (Integer.MAX_VALUE - i);
      }

      // Use quickselect to move the top k elements to the front of the array.
      if (k < n) {
        quickSelect(packed, 0, n - 1, k);
      }

      // Sort only the top k elements in descending order.
      // Since we want the top k, and quickSelect partitioned them to the front,
      // we sort the range [0, k-1].
      Arrays.sort(packed, 0, Math.min(k, n));
      // Arrays.sort is ascending, so we need to process from the end of the sorted range for 'top' values.
      // However, quickSelect/sort logic needs to be consistent. 
      // Let's refine: sort the whole range [0, k-1] and then read from k-1 down to 0 if we sorted ascending.
      // Or simpler: sort the range [0, k-1] and reverse or just adjust the checksum loop.
      
      h = 1469598103934665603L;
      int limit = Math.min(k, n);
      for (int i = 0; i < limit; i++) {
        // Because Arrays.sort is ascending, the largest values are at the end of the range [0, limit-1].
        long val = packed[limit - 1 - i];
        int score = (int) (val >> 32);
        int index = Integer.MAX_VALUE - (int) (val & 0xFFFFFFFFL);
        
        h ^= (((long) score) << 32) ^ (index & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }

  private static void quickSelect(long[] arr, int left, int right, int k) {
    int target = arr.length - k;
    while (left < right) {
      int pivotIndex = partition(arr, left, right);
      if (pivotIndex == target) {
        return;
      } else if (pivotIndex < target) {
        left = pivotIndex + 1;
      } else {
        right = pivotIndex - 1;
      }
    }
  }

  private static int partition(long[] arr, int left, int right) {
    long pivot = arr[left + (right - left) / 2];
    int i = left - 1;
    int j = right + 1;
    while (true) {
      do {
        i++;
      } while (arr[i] < pivot);
      do {
        j--;
      } while (arr[j] > pivot);
      if (i >= j) return j;
      long temp = arr[i];
      arr[i] = arr[j];
      arr[j] = temp;
    }
  }
}
