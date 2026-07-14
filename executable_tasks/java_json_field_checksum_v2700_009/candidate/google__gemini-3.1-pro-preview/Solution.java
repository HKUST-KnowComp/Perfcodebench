import java.util.*;

public class Solution {
  /**
   * Optimizes the extraction of fields 'u', 's', and 'f' from a fixed-format JSON string.
   * Instead of using a Regex Matcher, which is computationally expensive, we use
   * manual index tracking and character-to-integer conversion.
   */
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final long prime = 1099511628211L;

    for (String row : rows) {
      // Format: {"u":123,"s":456,"f":true,"tag":"x789"}
      
      // Parse 'u' starting at index 5 (after '{"u":')
      int i = 5;
      int u = 0;
      char c = row.charAt(i);
      while (c >= '0' && c <= '9') {
        u = u * 10 + (c - '0');
        i++;
        c = row.charAt(i);
      }

      // Skip ',"s":' (5 characters)
      i += 5;
      int s = 0;
      c = row.charAt(i);
      while (c >= '0' && c <= '9') {
        s = s * 10 + (c - '0');
        i++;
        c = row.charAt(i);
      }

      // Skip ',"f":' (5 characters)
      i += 5;
      // Check first character of boolean value ('t' for true, 'f' for false)
      boolean f = row.charAt(i) == 't';

      // Calculate value based on extracted fields
      long v = (u & 1023) + s + (f ? 17 : 0);
      
      // FNV-1a style hash update
      hash ^= (v + 1);
      hash *= prime;
    }
    
    return hash;
  }
}