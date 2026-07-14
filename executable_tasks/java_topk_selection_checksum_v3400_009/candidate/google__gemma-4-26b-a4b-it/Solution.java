import java.util.*;

public class Solution {
  /**
   * Optimized implementation using primitive long arrays to avoid object allocation.
   * We pack the score and index into a single long to allow efficient sorting/partitioning.
   * Since we need descending score and ascending index:
   * We transform the score to be 'comparable' in a single long: 
   * We use (long)score << 32 | (MAX_INT - index) to allow a single descending sort,
   * OR more simply: pack as (long)score << 32 | (index & 0xFFFFFFFFL) and use a custom comparator.
   * To avoid object overhead, we use a primitive long array and a Quickselect algorithm.
   */
  public static long run(int[] scores, int k, int iters) {
    int n = scores.length;
    long[] packed = new long[n];
    long h = 0;

    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < n; i++) {
        // Pack score in high 32 bits and index in low 32 bits.
        // To handle the requirement: descending score, then ascending index.
        // We can transform the index: index_transformed = (0x7FFFFFFF - index).
        // Then a standard descending sort on (score << 32 | index_transformed) works.
        // However, it's easier to just use a custom Quickselect logic on the packed longs.
        // Let's pack: score (high 32) and (0x7FFFFFFF - index) (low 32).
        // This way, higher score is 'larger', and for same score, smaller index is 'larger'.
        long scorePart = (long) scores[i] << 32;
        long indexPart = (0x7FFFFFFFL - (long) i) & 0xFFFFFFFFL;
        packed[i] = scorePart | indexPart;
      }

      // Quickselect to find the top k elements
      quickSelect(packed, 0, n - 1, k);

      // The top k elements are now in packed[0...k-1].
      // They are not necessarily sorted among themselves, so we must sort them.
      // We need them in the specific order: descending score, then ascending index.
      Arrays.sort(packed, 0, k);
      // Note: Arrays.sort on primitives is ascending. 
      // Our packing (score << 32 | (MAX-index)) means higher score is larger, 
      // and for same score, smaller index is larger. 
      // So the 'largest' elements are the ones we want. 
      // After ascending sort, the top k are at the end of the range [0, k-1]? No.
      // Let's re-evaluate: 
      // If we want top k, we want the k largest values in the packed array.
      // Quickselect(k) puts the k-th largest at index k-1, and everything > k-1 is to the left.
      // Wait, standard Quickselect puts the k-th element at its sorted position.
      // Let's use a simpler approach: pack as (score << 32 | (index_inverted)).
      // Then the k largest elements are the winners.
      
      // Re-sorting the top k to get them in descending order of 'packed' value.
      // Since we want descending score, then ascending index, and our packing 
      // makes (score, -index) a single comparable value, we sort descending.
      // Actually, the requirement is: descending score, then ascending index.
      // Let's use: packed[i] = ((long)scores[i] << 32) | (0x7FFFFFFFL - (long)i);
      // The largest values in this scheme are exactly the ones we want.
      // After quickselect, the k largest are in packed[0...k-1].
      // We sort them descending to match the original logic's order.
      
      // To avoid complexity, let's just sort the top k elements descending.
      // Since we used (score << 32 | (MAX-index)), the largest values are the winners.
      // We need to sort them descending to iterate from 0 to k-1.
      sortDescending(packed, 0, k - 1);

      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        long val = packed[i];
        int s = (int) (val >>> 32);
        int idx = (int) (0x7FFFFFFFL - (val & 0xFFFFFFFFL));
        h ^= (((long) s) << 32) ^ (idx & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }

  private static void quickSelect(long[] arr, int left, int right, int k) {
    int target = k - 1;
    while (left < right) {
      int pivotIndex = partition(arr, left, right);
      if (pivotIndex == target) return;
      if (pivotIndex < target) left = pivotIndex + 1;
      else right = pivotIndex - 1;
    }
  }

  private static int partition(long[] arr, int left, int right) {
    long pivot = arr[right];
    int i = left;
    for (int j = left; j < right; j++) {
      if (arr[j] > pivot) {
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

  private static void sortDescending(long[] arr, int left, int right) {
    // Simple insertion sort for the small k range
    for (int i = left + 1; i <= right; i++) {
      long key = arr[i];
      int j = i - 1;
      while (j >= left && arr[j] < key) {
        arr[j + 1] = arr[j];
        j--;
      }
      arr[j + 1] = key;
    }
  }
}