import java.util.List;

public class Solution {
  /**
   * Optimizes the processing of structured rows by avoiding expensive string operations.
   * Uses manual character scanning to extract integers and compute the checksum.
   */
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final long fnvPrime = 1099511628211L;

    for (String row : rows) {
      int len = row.length();
      
      // Field 1: starts at index 2 (skips prefix of length 2)
      int i = 2;
      int u = 0;
      char c;
      while (i < len && (c = row.charAt(i)) != '&') {
        u = u * 10 + (c - '0');
        i++;
      }
      
      // Field 2: starts 6 characters after the first '&'
      i += 7; // skip '&' and prefix of length 6
      int s = 0;
      while (i < len && (c = row.charAt(i)) != '&') {
        s = s * 10 + (c - '0');
        i++;
      }
      
      // Field 3: starts 5 characters after the second '&'
      i += 6; // skip '&' and prefix of length 5
      int f = 0;
      while (i < len) {
        f = f * 10 + (row.charAt(i) - '0');
        i++;
      }

      // Baseline logic: v = (u & 2047) + s + f * 19L
      long v = (u & 2047) + (long) s + (long) f * 19L;
      
      // FNV-1a style hash update
      hash ^= (v + 1);
      hash *= fnvPrime;
    }
    
    return hash;
  }
}