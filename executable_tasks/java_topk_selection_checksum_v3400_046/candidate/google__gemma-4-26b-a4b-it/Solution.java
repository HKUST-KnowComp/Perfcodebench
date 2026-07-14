import java.util.*;

public class Solution {
  public static long run(int[] scores, int k, int iters) {
    int n = scores.length;
    long[] packed = new long[n];
    long h = 1469598103934665603L;

    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < n; i++) {
        // Pack score and index into a long to avoid object allocation.
        // We want descending score, then ascending index.
        // To use standard ascending sort on longs to get our desired order:
        // 1. Invert the score: (Integer.MAX_VALUE - score)
        // 2. Keep index as is.
        // However, to handle negative scores and simplify, we use a bit-trick:
        // We want higher score to have a 'smaller' long value.
        // Let's map score to a sortable space: 
        // We'll use: (long)(~score) << 32 | (index & 0xFFFFFFFFL)
        // But wait, the requirement is: score DESC, index ASC.
        // Let's use: (long)(Integer.MAX_VALUE - score) << 32 | (index & 0xFFFFFFFFL)
        // This is tricky with negative scores. 
        // Better: Use a custom comparator logic or transform score to unsigned-like.
        // Let's use: (long) (score ^ Integer.MIN_VALUE) << 32 | (index & 0xFFFFFFFFL)
        // This maps the smallest score to 0 and largest to 2^32-1. 
        // Since we want DESC score, we want the LARGEST score to be the SMALLEST long.
        // So: (long) (Integer.MAX_VALUE - score) is not enough for all ints.
        // Let's use: (long) (score - Integer.MAX_VALUE - 1) is also messy.
        // Correct way to pack for DESC score, ASC index using ASC sort:
        // We want score high -> long low. 
        // Let's use: (long) (~score) << 32 | (index & 0xFFFFFFFFL)
        // If score is 10, ~score is -11. If score is 5, ~score is -6. 
        // -11 < -6. So higher score results in a smaller ~score.
        // For index: we want ASC. So index 0 < index 1.
        // So: packed[i] = ((long) ~scores[i] << 32) | (index & 0xFFFFFFFFL);
        // Wait, index is unsigned in the bitmask. Let's ensure index is treated as positive.
        // Let's use: (long) (~scores[i]) << 32 | (index & 0xFFFFFFFFL);
        // Let's test: 
        // Score 10, Index 1 -> ~10 = ...11110101. Long: ...0101...0001
        // Score 5, Index 2  -> ~5  = ...11111010. Long: ...1010...0010
        // Since ~10 < ~5, the higher score comes first in an ascending sort.
        // For index: if scores are equal, ~score is equal. Then we compare index.
        // index 1 < index 2, so index 1 comes first. Perfect.
        packed[i] = ((long) ~scores[i] << 32) | (i & 0xFFFFFFFFL);
      }

      // Use Quickselect to find the top k elements (the k smallest packed longs)
      quickSelect(packed, 0, n - 1, k);

      // The top k are now in packed[0...k-1], but they are not necessarily sorted.
      // The requirement says: "checksum over the ordered winners".
      // The original code sorted the WHOLE array. We must sort the top k.
      Arrays.sort(packed, 0, k);

      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        long p = packed[i];
        int s = (int) (p >> 32);
        s = ~s; // undo the ~score
        int idx = (int) (p & 0xFFFFFFFFL);
        h ^= (((long) s) << 32) ^ (idx & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }

  private static void quickSelect(long[] arr, int left, int right, int k) {
    while (left < right) {
      int pivotIndex = partition(arr, left, right);
      if (pivotIndex == k) return;
      if (pivotIndex < k) {
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
      do { i++; } while (arr[i] < pivot);
      do { j--; } while (arr[j] > pivot);
      if (i >= j) return j;
      long temp = arr[i];
      arr[i] = arr[j];
      arr[j] = temp;
    }
  }
}