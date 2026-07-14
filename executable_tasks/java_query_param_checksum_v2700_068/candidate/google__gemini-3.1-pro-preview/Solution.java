import java.util.List;

public class Solution {
  /**
   * Optimizes the processing of structured rows to calculate a checksum.
   * Avoids expensive String operations like split() and substring() by manually
   * scanning the characters and parsing integers in-place.
   */
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final long fnvPrime = 1099511628211L;

    for (String row : rows) {
      int len = row.length();
      int u = 0;
      int s = 0;
      int f = 0;
      
      // The baseline logic implies specific offsets for values:
      // parts[0] starts at index 0, value starts at index 2 (after "u=")
      // parts[1] starts after first '&', value starts at index 6 relative to part start
      // parts[2] starts after second '&', value starts at index 5 relative to part start

      int i = 2; // Skip "u="
      // Parse u
      while (i < len) {
        char c = row.charAt(i);
        if (c == '&') break;
        u = u * 10 + (c - '0');
        i++;
      }
      
      // Skip '&' and the next 6 characters (e.g., "status")
      i += 7;
      
      // Parse s
      while (i < len) {
        char c = row.charAt(i);
        if (c == '&') break;
        s = s * 10 + (c - '0');
        i++;
      }
      
      // Skip '&' and the next 5 characters (e.g., "flag=")
      i += 6;
      
      // Parse f
      while (i < len) {
        char c = row.charAt(i);
        f = f * 10 + (c - '0');
        i++;
      }

      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= fnvPrime;
    }
    return hash;
  }
}