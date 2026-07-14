import java.util.*;

public class Solution {
  public static long run(int[] scores, int k, int iters) {
    int n = scores.length;
    long[] packed = new long[n];
    long h = 1469598103934665603L;

    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < n; i++) {
        // Pack score and index into a long for efficient sorting.
        // We want descending score, then ascending index.
        // To use standard ascending sort, we transform score to its complement or negate.
        // However, to handle the 'descending score, ascending index' logic:
        // We use: (-(long)score << 32) | (index & 0xFFFFFFFFL)
        // This way, a smaller long value corresponds to a higher score.
        // If scores are equal, a smaller index results in a smaller long value.
        packed[i] = (-(long) scores[i] << 32) | (i & 0xFFFFFFFFL);
      }

      // Use Quickselect to move the top k elements to the front
      quickSelect(packed, 0, n - 1, k);

      // The top k elements are now in packed[0...k-1], but they are not necessarily ordered.
      // The problem requires the checksum over the ORDERED winners.
      Arrays.sort(packed, 0, k);

      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        long p = packed[i];
        int score = (int) (- (p >>> 32));
        int index = (int) (p & 0xFFFFFFFFL);
        h ^= (((long) score) << 32) ^ (index & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }

  private static void quickSelect(long[] arr, int left, int right, int k) {
    int target = k - 1;
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
      do { i++; } while (arr[i] < pivot);
      do { j--; } while (arr[j] > pivot);
      if (i >= j) return j;
      long temp = arr[i];
      arr[i] = arr[j];
      arr[j] = temp;
    }
  }
}