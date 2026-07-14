import java.util.*;

public class Solution {
  public static long run(int[] scores, int k, int iters) {
    int n = scores.length;
    // Pack score and index into a single long for primitive processing.
    // We want descending score, then ascending index.
    // To use standard long comparison (ascending), we transform the score:
    // New Score = (MAX_INT - original_score). 
    // However, to handle negative scores and avoid overflow, we use a bit-trick:
    // We want higher score to have a 'smaller' bit representation for ascending sort.
    // Or more simply: pack as (inverted_score << 32) | (index).
    // Since we need descending score, we can use (Integer.MAX_VALUE - score) or similar.
    // Let's use: long val = ((long)score << 32) | (index & 0xFFFFFFFFL);
    // But we need to sort by score DESC, then index ASC.
    // Let's pack: [32 bits: score (inverted)] [32 bits: index]
    // To invert score for descending order: score_bits = ~score_bits (if we treat as unsigned)
    // Actually, a simpler way: pack as (long)score << 32 | (index & 0xFFFFFFFFL).
    // Then we need to sort such that score is high to low, then index low to high.
    
    long[] packed = new long[n];
    long h = 0;

    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < n; i++) {
        // We want to sort by score DESC, then index ASC.
        // In a standard ascending sort of longs:
        // We want higher score to appear 'smaller'.
        // We want lower index to appear 'smaller' if scores are equal.
        // Let's map score 's' to 'unsigned_max - s'.
        // Since scores are ints, we can use: (long)score << 32 | (index & 0xFFFFFFFFL)
        // and then use a custom comparator or transform the bits.
        
        // Transformation for ascending sort to achieve (score DESC, index ASC):
        // 1. Flip the sign bit of the score to reverse order.
        // 2. Keep index as is.
        // But wait, the simplest way to handle the 'score DESC, index ASC' requirement 
        // in a single long for ascending sort is:
        // bits = ((long)score << 32) | (index & 0xFFFFFFFFL);
        // Then we need to sort such that score is high to low.
        // Let's use: long val = ((long)score << 32) | (index & 0xFFFFFFFFL);
        // To sort DESC score, ASC index: 
        // We can use: long val = ((long)score << 32) | (index & 0xFFFFFFFFL);
        // And use a custom Quickselect/Sort logic.
        
        // Let's use a more robust packing for ascending sort:
        // We want (score1, index1) < (score2, index2) if 
        // score1 > score2 OR (score1 == score2 AND index1 < index2).
        // Let's map score 's' to 'unsigned_int_max - s'.
        // Since we don't know the range, let's use: 
        // long val = ((long)score ^ 0x80000000L) << 32 | (index & 0xFFFFFFFFL);
        // This doesn't quite work for the index part. 
        
        // Let's just use the raw values and a custom Quickselect to avoid bit-math confusion.
        // Actually, the most performant way is to pack: 
        // long val = ((long)score << 32) | (index & 0xFFFFFFFFL);
        // And use a custom comparison in Quickselect.
        packed[i] = ((long)scores[i] << 32) | (i & 0xFFFFFFFFL);
      }

      // Quickselect to find the top k elements.
      // We need the k elements that would be at the start of the sorted array.
      quickSelect(packed, 0, n - 1, k);

      // The top k elements are now in packed[0...k-1], but they are not necessarily sorted.
      // The requirement says: "checksum over the ordered winners".
      // The original code sorted the whole array. So we must sort the top k.
      Arrays.sort(packed, 0, k, (a, b) -> {
        long s1 = a >>> 32;
        long s2 = b >>> 32;
        if (s1 != s2) return Long.compare(s2, s1); // Descending score
        return Long.compare(a & 0xFFFFFFFFL, b & 0xFFFFFFFFL); // Ascending index
      });

      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        long s = packed[i] >>> 32;
        long idx = packed[i] & 0xFFFFFFFFL;
        h ^= (s << 32) ^ idx;
        h *= 1099511628211L;
      }
    }
    return h;
  }

  private static void quickSelect(long[] arr, int left, int right, int k) {
    while (right > left) {
      int pivotIndex = partition(arr, left, right);
      if (pivotIndex == k) return;
      if (pivotIndex < k) left = pivotIndex + 1;
      else right = pivotIndex - 1;
    }
  }

  private static int partition(long[] arr, int left, int right) {
    // Use middle element as pivot to avoid O(n^2) on sorted data
    int mid = left + (right - left) / 2;
    long pivot = arr[mid];
    swap(arr, mid, right);
    int storeIndex = left;
    for (int i = left; i < right; i++) {
      if (compare(arr[i], pivot) < 0) {
        swap(arr, i, storeIndex);
        storeIndex++;
      }
    }
    swap(arr, storeIndex, right);
    return storeIndex;
  }

  private static int compare(long a, long b) {
    // Returns < 0 if a should come before b in the sorted order (score DESC, index ASC)
    long s1 = a >>> 32;
    long s2 = b >>> 32;
    if (s1 != s2) return Long.compare(s2, s1);
    return Long.compare(a & 0xFFFFFFFFL, b & 0xFFFFFFFFL);
  }

  private static void swap(long[] arr, int i, int j) {
    long temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
  }
}