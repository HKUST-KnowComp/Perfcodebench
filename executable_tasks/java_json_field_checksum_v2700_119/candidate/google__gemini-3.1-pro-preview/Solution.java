import java.util.*;

public class Solution {
  /**
   * Optimizes the processing of structured JSON-like rows by manually parsing
   * required fields (u, s, f) instead of using expensive regular expressions.
   * 
   * The format is assumed to be: {"u":<int>,"s":<int>,"f":<bool>,"tag":"x<int>"}
   */
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final long prime = 1099511628211L;

    for (String row : rows) {
      int i = 5; // Start after '{"u":'
      
      // Parse integer 'u'
      int u = 0;
      char c = row.charAt(i);
      while (c >= '0' && c <= '9') {
        u = u * 10 + (c - '0');
        c = row.charAt(++i);
      }

      // Skip ',"s":' (5 characters)
      i += 5;
      
      // Parse integer 's'
      int s = 0;
      c = row.charAt(i);
      while (c >= '0' && c <= '9') {
        s = s * 10 + (c - '0');
        c = row.charAt(++i);
      }

      // Skip ',"f":' (5 characters)
      i += 5;
      
      // Parse boolean 'f' (check if it starts with 't' for true)
      boolean f = row.charAt(i) == 't';

      // Calculate checksum component
      long v = (u & 1023) + s + (f ? 17 : 0);
      
      // FNV-1a style hash update
      hash ^= (v + 1);
      hash *= prime;
    }
    
    return hash;
  }
}