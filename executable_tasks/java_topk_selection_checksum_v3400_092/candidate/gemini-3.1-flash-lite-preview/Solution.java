import java.util.*;

public class Solution {
  public static long run(int[] scores, int k, int iters) {
    int n = scores.length;
    long[] packed = new long[n];
    long h = 0;
    
    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < n; i++) {
        // Pack score in high 32 bits, index in low 32 bits.
        // To sort descending by score then ascending by index:
        // We negate score for natural ascending sort, or use custom comparator.
        // Using a custom long array sort is faster than object arrays.
        packed[i] = ((long) scores[i] << 32) | (i & 0xffffffffL);
      }
      
      // Sort using a custom comparator logic on longs
      // Since we want descending score, ascending index:
      // We can't use Arrays.sort(long[]) directly for this custom order easily.
      // Use a boxed sort or a primitive quicksort. Given constraints, we implement a quicksort.
      quickSort(packed, 0, n - 1);
      
      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        long val = packed[i];
        h ^= (val ^ (val >>> 32));
        h *= 1099511628211L;
      }
    }
    return h;
  }

  private static void quickSort(long[] a, int left, int right) {
    if (left >= right) return;
    long pivot = a[left + (right - left) / 2];
    int i = left, j = right;
    while (i <= j) {
      while (compare(a[i], pivot) < 0) i++;
      while (compare(a[j], pivot) > 0) j--;
      if (i <= j) {
        long temp = a[i]; a[i] = a[j]; a[j] = temp;
        i++; j--;
      }
    }
    quickSort(a, left, j);
    quickSort(a, i, right);
  }

  private static int compare(long a, long b) {
    int scoreA = (int) (a >>> 32);
    int scoreB = (int) (b >>> 32);
    if (scoreA != scoreB) return Integer.compare(scoreB, scoreA);
    return Integer.compare((int) a, (int) b);
  }
}