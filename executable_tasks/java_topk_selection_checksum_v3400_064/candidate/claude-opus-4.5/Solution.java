import java.util.*;

public class Solution {

  public static long run(int[] scores, int k, int iters) {
    int n = scores.length;
    long[] packed = new long[n];
    long h = 0;
    
    for (int iter = 0; iter < iters; iter++) {
      // Pack score and index into a single long for cache efficiency
      // High 32 bits: negated score (so ascending sort gives descending scores)
      // Low 32 bits: index (for tie-breaking)
      for (int i = 0; i < n; i++) {
        packed[i] = ((long)(-scores[i]) << 32) | (i & 0xffffffffL);
      }
      
      // Partial selection: partition so that smallest k elements (by packed value) are in [0, k)
      quickSelect(packed, 0, n - 1, k);
      
      // Sort only the top-k elements
      Arrays.sort(packed, 0, k);
      
      // Compute checksum
      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        long p = packed[i];
        int score = -(int)(p >> 32);
        int index = (int)p;
        h ^= (((long) score) << 32) ^ (index & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
  
  private static void quickSelect(long[] arr, int left, int right, int k) {
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