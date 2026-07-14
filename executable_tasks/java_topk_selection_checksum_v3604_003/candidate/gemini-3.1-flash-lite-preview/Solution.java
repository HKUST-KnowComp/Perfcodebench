import java.util.*;

public class Solution {
  public static long run(int[] scores, int k, int iters) {
    int n = scores.length;
    long[] packed = new long[n];
    for (int i = 0; i < n; i++) {
      // Pack score in high 32 bits, index in low 32 bits.
      // To sort descending by score then ascending by index:
      // We store score as is, but negate index to make it sortable.
      // Actually, simpler: store score in high bits, index in low bits.
      // Since we want descending score, we can negate score or use custom comparator.
      // Using a custom long array sort is faster.
      packed[i] = ((long) scores[i] << 32) | (i & 0xffffffffL);
    }

    long h = 0;
    for (int iter = 0; iter < iters; iter++) {
      // Sort packed array. To get descending score, we use a custom sort logic.
      // Since we need to preserve the original scores/indices, we copy.
      long[] current = packed.clone();
      Arrays.sort(current);
      
      h = 1469598103934665603L;
      // The array is sorted ascending by score (high bits) then index (low bits).
      // We need top k (highest scores).
      for (int i = 0; i < k; i++) {
        long val = current[n - 1 - i];
        int score = (int) (val >> 32);
        int index = (int) (val & 0xffffffffL);
        h ^= (((long) score) << 32) ^ (index & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}