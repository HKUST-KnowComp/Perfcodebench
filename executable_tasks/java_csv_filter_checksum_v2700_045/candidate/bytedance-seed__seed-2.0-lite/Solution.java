import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int foundCommas = 0;
      int c1 = 0, c2 = 0, c3 = 0;
      final int len = row.length();
      // Scan only to find first 4 commas, stop early once we have all needed positions
      for (int i = 0; i < len && foundCommas < 4; i++) {
        if (row.charAt(i) == ',') {
          foundCommas++;
          if (foundCommas == 2) c1 = i;
          else if (foundCommas == 3) c2 = i;
          else if (foundCommas == 4) c3 = i;
        }
      }
      // Parse b (parts[2]) directly from char sequence
      int b = 0;
      for (int i = c1 + 1; i < c2; i++) {
        b = b * 10 + (row.charAt(i) - '0');
      }
      // Parse c (parts[3]) directly from char sequence
      int c = 0;
      for (int i = c2 + 1; i < c3; i++) {
        c = c * 10 + (row.charAt(i) - '0');
      }
      // Check if parts[4] is exactly "hot" to match original equals() behavior
      boolean hot = false;
      final int hotStart = c3 + 1;
      if (len >= hotStart + 3) {
        if (row.charAt(hotStart) == 'h' && row.charAt(hotStart + 1) == 'o' && row.charAt(hotStart + 2) == 't') {
          if (hotStart + 3 == len || row.charAt(hotStart + 3) == ',') {
            hot = true;
          }
        }
      }
      // Exact same checksum calculation as original to preserve correctness
      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}