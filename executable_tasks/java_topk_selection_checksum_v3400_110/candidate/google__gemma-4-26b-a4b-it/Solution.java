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
        // To use standard ascending sort, we transform score to (MAX_INT - score).
        // However, to handle negative scores and avoid overflow, we use a bit-trick:
        // We map the score to a sortable long where higher score = smaller value.
        // Since we need descending score: we use (long)score and flip bits or use a comparator logic.
        // Let's use: high 32 bits = (score ^ 0x80000000) inverted, low 32 bits = index.
        // Actually, simpler: pack as (long)score << 32 | (index & 0xFFFFFFFFL).
        // But we need custom order: score DESC, index ASC.
        // Let's pack: [32 bits: inverted score] [32 bits: index].
        // To invert score for descending order: score_transformed = ~score;
        // But we must handle the sign bit. A robust way: 
        // Use a long where high 32 bits are the score, but we flip the sign bit to make it sortable.
        // Or just use a custom quickselect on the primitive array.
        packed[i] = ((long) scores[i] << 32) | (i & 0xFFFFFFFFL);
      }

      // We need top k. The criteria: score DESC, index ASC.
      // Let's use a custom quickselect/sort logic on the packed longs.
      // Since we need the exact top k in order, we can use a partial sort.
      // For simplicity and performance in Java, we'll use a custom quicksort on the packed array
      // but we must implement the specific comparator logic.
      
      quickSort(packed, 0, n - 1);

      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        int s = (int) (packed[i] >> 32);
        int idx = (int) (packed[i] & 0xFFFFFFFFL);
        h ^= (((long) s) << 32) ^ (idx & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }

  private static void quickSort(long[] a, int left, int right) {
    if (left >= right) return;
    int pivotIdx = partition(a, left, right);
    quickSort(a, left, pivotIdx);
    quickSort(a, pivotIdx + 1, right);
  }

  private static int partition(long[] a, int left, int right) {
    long pivot = a[left + (right - left) / 2];
    int i = left - 1;
    int j = right + 1;
    while (true) {
      do { i++; } while (compare(a[i], pivot) < 0);
      do { j--; } while (compare(a[j], pivot) > 0);
      if (i >= j) return j;
      long temp = a[i];
      a[i] = a[j];
      a[j] = temp;
    }
  }

  private static int compare(long a, long b) {
    int s1 = (int) (a >> 32);
    int s2 = (int) (b >> 32);
    if (s1 != s2) return Integer.compare(s2, s1); // Descending score
    int i1 = (int) (a & 0xFFFFFFFFL);
    int i2 = (int) (b & 0xFFFFFFFFL);
    return Integer.compare(i1, i2); // Ascending index
  }
}