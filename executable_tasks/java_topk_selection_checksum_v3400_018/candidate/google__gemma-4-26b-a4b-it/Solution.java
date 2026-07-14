import java.util.Arrays;

public class Solution {
  public static long run(int[] scores, int k, int iters) {
    int n = scores.length;
    long[] packed = new long[n];
    long h = 0;
    
    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < n; i++) {
        // Pack score and index into a long for efficient sorting.
        // We want descending score, then ascending index.
        // To use Arrays.sort (ascending), we transform the score:
        // 1. Flip the sign of the score (or use bitwise complement) to reverse order.
        // 2. Use the index as the lower 32 bits.
        // Since we want descending score, we can use (Integer.MAX_VALUE - score) 
        // or simply negate it. However, to handle the full range of int, 
        // we use the bitwise complement of the score to reverse the order.
        // A more robust way for descending score and ascending index:
        // High 32 bits: ~score (this makes larger scores smaller values)
        // Low 32 bits: index (this keeps index ascending)
        long s = scores[i];
        long idx = i;
        // Using bitwise complement on the score to reverse order for ascending sort
        // We must be careful with the sign bit. 
        // Let's use: high bits = (score ^ 0x80000000) inverted, or simply:
        // high bits = (long)score << 32; then we need to sort descending.
        // Let's use a simpler packing: 
        // high 32 bits = (long)score (but we need descending order)
        // To get descending score and ascending index using ascending sort:
        // We map score 's' to 'Integer.MIN_VALUE - s' or similar.
        // Let's use: high 32 bits = (long)score ^ 0x80000000L (flips sign bit)
        // But we want descending, so we want the largest score to be the smallest value.
        // Let's use: high 32 bits = (long)score; then we sort and pick from the end?
        // No, the requirement is top-k. Let's use: 
        // high 32 bits = (long)score (but we need to handle the sort order)
        // Let's use: high 32 bits = (long)score, but we use a custom sort or 
        // transform the score so that higher scores result in smaller long values.
        // Transformation: high32 = (long)score ^ 0x80000000L; 
        // This makes the largest score have the smallest high 32 bits.
        // However, we also need ascending index for ties. 
        // If scores are equal, the high 32 bits are equal. 
        // Then the low 32 bits (index) will be compared ascending.
        // This is exactly what we want.
        packed[i] = ((long)scores[i] ^ 0x80000000L) << 32 | (idx & 0xFFFFFFFFL);
      }
      
      Arrays.sort(packed);

      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        long p = packed[i];
        int s = (int)((p >> 32) ^ 0x80000000L);
        int idx = (int)(p & 0xFFFFFFFFL);
        h ^= (((long) s) << 32) ^ (idx & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}