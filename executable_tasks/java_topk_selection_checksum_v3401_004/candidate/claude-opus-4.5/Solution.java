import java.util.*;

public class Solution {
  public static long run(int[] scores, int k, int iters) {
    int n = scores.length;
    long[] combined = new long[n];
    long h = 0;
    
    for (int iter = 0; iter < iters; iter++) {
      // Pack score and index into single long: high 32 bits = score, low 32 bits = index
      // For descending score order, we negate the score
      for (int i = 0; i < n; i++) {
        combined[i] = ((long)(-scores[i]) << 32) | (i & 0xffffffffL);
      }
      
      // Quickselect to partition so that smallest k elements (by negated score) are in [0, k)
      quickselect(combined, 0, n - 1, k);
      
      // Sort only the top-k elements
      Arrays.sort(combined, 0, k);
      
      // Compute checksum
      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        long c = combined[i];
        int score = -(int)(c >> 32);
        int index = (int)c;
        h ^= (((long) score) << 32) ^ (index & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
  
  private static void quickselect(long[] arr, int left, int right, int k) {
    while (left < right) {
      int pivotIndex = partition(arr, left, right);
      if (pivotIndex == k) {
        return;
      } else if (pivotIndex < k) {
        left = pivotIndex + 1;
      } else {
        right = pivotIndex - 1;
      }
    }
  }
  
  private static int partition(long[] arr, int left, int right) {
    // Median-of-three pivot selection
    int mid = left + (right - left) / 2;
    if (arr[mid] < arr[left]) {
      long tmp = arr[left]; arr[left] = arr[mid]; arr[mid] = tmp;
    }
    if (arr[right] < arr[left]) {
      long tmp = arr[left]; arr[left] = arr[right]; arr[right] = tmp;
    }
    if (arr[mid] < arr[right]) {
      long tmp = arr[mid]; arr[mid] = arr[right]; arr[right] = tmp;
    }
    long pivot = arr[right];
    
    int i = left - 1;
    for (int j = left; j < right; j++) {
      if (arr[j] <= pivot) {
        i++;
        long tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
      }
    }
    long tmp = arr[i + 1];
    arr[i + 1] = arr[right];
    arr[right] = tmp;
    return i + 1;
  }
}