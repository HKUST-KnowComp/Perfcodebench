import java.util.*;

public class Solution {
  public static long run(int[] scores, int k, int iters) {
    int n = scores.length;
    long[] packed = new long[n];
    long h = 0;

    for (int iter = 0; iter < iters; iter++) {
      // Pack score and index into a single long.
      // We want: higher score first, then lower index first.
      // By using ~score in the high bits, a natural ascending sort 
      // puts higher scores first. The index in the low bits ensures 
      // lower indices come first for identical scores.
      for (int i = 0; i < n; i++) {
        packed[i] = ((long) (~scores[i]) << 32) | (i & 0xFFFFFFFFL);
      }

      // Use quickselect to partition the top k elements to the front.
      if (k < n) {
        select(packed, 0, n - 1, k);
      }

      // Sort only the top k elements to satisfy the checksum order requirement.
      Arrays.sort(packed, 0, k);

      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        long p = packed[i];
        int score = ~(int) (p >>> 32);
        int index = (int) (p & 0xFFFFFFFFL);
        h ^= (((long) score) << 32) ^ (index & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }

  private static void select(long[] arr, int left, int right, int k) {
    while (left < right) {
      int pivotIndex = partition(arr, left, right);
      if (pivotIndex >= k) {
        right = pivotIndex - 1;
      } else {
        left = pivotIndex + 1;
      }
    }
  }

  private static int partition(long[] arr, int left, int right) {
    int mid = left + (right - left) / 2;
    long pivot = arr[mid];
    swap(arr, mid, right);
    int storeIndex = left;
    for (int i = left; i < right; i++) {
      if (arr[i] < pivot) {
        swap(arr, i, storeIndex);
        storeIndex++;
      }
    }
    swap(arr, storeIndex, right);
    return storeIndex;
  }

  private static void swap(long[] arr, int i, int j) {
    long temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
  }
}
