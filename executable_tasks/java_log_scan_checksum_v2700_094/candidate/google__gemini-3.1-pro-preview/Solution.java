import java.util.*;

public class Solution {
  /**
   * Optimizes the log scanning process by replacing regex with manual parsing.
   * The format is fixed: src=S(\d+) lat=(\d+) code=(\d+) ok=(yes|no)
   */
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final long fnvPrime = 1099511628211L;

    for (String row : rows) {
      int n = row.length();
      int i = 5; // Skip "src=S"

      // Parse src
      int src = 0;
      char c;
      while (i < n && (c = row.charAt(i)) >= '0' && c <= '9') {
        src = src * 10 + (c - '0');
        i++;
      }

      // Skip " lat="
      i += 5;
      int lat = 0;
      while (i < n && (c = row.charAt(i)) >= '0' && c <= '9') {
        lat = lat * 10 + (c - '0');
        i++;
      }

      // Skip " code="
      i += 6;
      int code = 0;
      while (i < n && (c = row.charAt(i)) >= '0' && c <= '9') {
        code = code * 10 + (c - '0');
        i++;
      }

      // Skip " ok="
      i += 4;
      // Check if "yes" or "no"
      boolean ok = row.charAt(i) == 'y';

      // Calculate checksum component
      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      
      // FNV-1a style hash update
      hash ^= (v + 1);
      hash *= fnvPrime;
    }
    return hash;
  }
}