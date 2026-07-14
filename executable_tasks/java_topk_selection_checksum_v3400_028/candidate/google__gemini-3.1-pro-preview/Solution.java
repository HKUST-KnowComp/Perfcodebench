import java.util.*;

public class Solution {
  public static long run(int[] scores, int k, int iters) {
    int n = scores.length;
    long[] packed = new long[n];
    long h = 0;

    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < n; i++) {
        // Pack score (high 32 bits) and index (low 32 bits).
        // To handle descending score and ascending index in a single long comparison:
        // 1. Score in high bits (natural order for descending).
        // 2. Index inverted in low bits (so smaller index becomes larger value).
        packed[i] = ((long) scores[i] << 32) | (0xFFFFFFFFL - i);
      }

      if (k < n) {
        quickSelect(packed, 0, n - 1, k);
      }

      // Sort only the top k elements
      Arrays.sort(packed, 0, Math.min(k, n));

      // Reverse the top k because Arrays.sort is ascending and we need descending
      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        // The top k elements are at the end of the sorted range [0, k) if we sort ascending,
        // but quickSelect/sort logic here needs to be careful.
        // Let's sort the whole array or use a descending sort logic.
        // Actually, Arrays.sort(packed, 0, k) sorts the top-k candidates ascending.
        // The largest values (top scores) are at the end of the range [0, k).
        long val = packed[k - 1 - i];
        int score = (int) (val >>> 32);
        int index = (int) (0xFFFFFFFFL - (val & 0xFFFFFFFFL));
        
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
    long pivot = arr[right];
    int i = left;
    for (int j = left; j < right; j++) {
      if (arr[j] <= pivot) {
        long temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
        i++;
      }
    }
    long temp = arr[i];
    arr[i] = arr[right];
    arr[right] = temp;
    return i;
  }
}
