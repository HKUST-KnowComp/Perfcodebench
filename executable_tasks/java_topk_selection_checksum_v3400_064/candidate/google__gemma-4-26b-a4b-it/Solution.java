import java.util.Arrays;

public class Solution {
  public static long run(int[] scores, int k, int iters) {
    int n = scores.length;
    long[] packed = new long[n];
    long h = 1469598103934665603L;

    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < n; i++) {
        // Pack score and index into a long for efficient sorting.
        // We want descending score, then ascending index.
        // To use natural ascending order of longs:
        // 1. Negate score (or use bitwise complement) to flip order.
        // 2. Use index as is.
        // However, since scores can be negative, we use a transformation:
        // We want higher score first. Let's map score to (Integer.MAX_VALUE - score).
        // Or more simply: pack as (long)score << 32 | (index & 0xFFFFFFFFL).
        // But we need descending score. Let's use: 
        // (long) (~score) << 32 | (index & 0xFFFFFFFFL) is tricky with negatives.
        // Let's use: (long) (-(long)scores[i]) << 32 | (index & 0xFFFFFFFFL).
        // Wait, the requirement is: score DESC, then index ASC.
        // Let's use: (long) (0xFFFFFFFFL - (unsigned)score) is not quite right.
        // Correct way: pack as (long) (-(long)scores[i]) << 32 | (index & 0xFFFFFFFFL).
        // But scores are ints. Let's use: 
        // long val = ((long) scores[i] << 32) | (index & 0xFFFFFFFFL);
        // To sort DESC score, ASC index: 
        // We can use a custom comparator or transform the bits.
        // Let's transform: 
        // NewScore = -score (if we treat it as long)
        // NewIndex = index
        // This works for descending score. 
        // To handle the full range of int, we use: 
        // long bitScore = (long)scores[i];
        // long packedVal = (bitScore ^ 0x80000000L) << 32 | (index & 0xFFFFFFFFL);
        // This maps the smallest int to 0 and largest to 0xFFFFFFFF. 
        // To get DESC score, we want the largest score to have the smallest bitScore.
        // Let's use: 
        // long bitScore = (long)scores[i];
        // long packedVal = (~bitScore << 32) | (index & 0xFFFFFFFFL);
        // Wait, if score is 10, ~10 is ...11110101. If score is 11, ~11 is ...11110100.
        // So ~11 < ~10. This gives DESC score. 
        // For index: we want ASC index. index 0 < index 1. 
        // So: packedVal = ((long) ~scores[i] << 32) | (index & 0xFFFFFFFFL);
        // Let's double check: 
        // Score 10, Index 1 -> ~10 << 32 | 1
        // Score 10, Index 2 -> ~10 << 32 | 2
        // Score 11, Index 1 -> ~11 << 32 | 1
        // Since ~11 < ~10, (Score 11, Index 1) comes before (Score 10, Index 1).
        // This is correct for DESC score, ASC index.
        packed[i] = ((long) ~scores[i] << 32) | (index & 0xFFFFFFFFL);
      }
      
      Arrays.sort(packed);

      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        long p = packed[i];
        int s = (int) (p >> 32);
        s = ~s; // undo the ~ 
        int idx = (int) (p & 0xFFFFFFFFL);
        h ^= (((long) s) << 32) ^ (idx & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}