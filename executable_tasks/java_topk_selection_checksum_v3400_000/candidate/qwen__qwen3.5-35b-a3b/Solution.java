import java.util.*;

public class Solution {
  public static long run(int[] scores, int k, int iters) {
    if (iters == 0) return 0;
    
    int n = scores.length;
    long[] packed = new long[n];
    
    // Pack score and index into a long for fast primitive sorting.
    // We want descending score, then ascending index.
    // Standard sort is ascending. So we negate the score (bitwise NOT) to invert order.
    // ~score is smaller when score is larger.
    // If scores are equal, ~score is equal, so it sorts by index ascending.
    for (int i = 0; i < n; i++) {
      packed[i] = ((long) (~scores[i]) << 32) | (i & 0xffffffffL);
    }
    
    Arrays.sort(packed);
    
    long h = 1469598103934665603L;
    long fnvPrime = 1099511628211L;
    
    // Compute hash over top k
    for (int i = 0; i < k; i++) {
      long val = packed[i];
      int score = ~(int)(val >>> 32);
      int index = (int) val;
      
      h ^= (((long) score) << 32) ^ (index & 0xffffffffL);
      h *= fnvPrime;
    }
    
    return h;
  }
}