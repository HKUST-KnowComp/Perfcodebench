import java.util.*;

public class Solution {
  /**
   * Optimizes the processing of structured JSON rows by avoiding Regex and String allocations.
   * The logic manually parses the 'u', 's', and 'f' fields based on their fixed positions
   * and delimiters in the input format: {"u":(\d+),"s":(\d+),"f":(true|false),"tag":"x\d+"}
   */
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final long prime = 1099511628211L;

    for (String row : rows) {
      int len = row.length();
      // Skip '{"u":' (5 chars)
      int i = 5;
      
      // Parse 'u'
      int u = 0;
      char c = row.charAt(i);
      while (c != ',') {
        u = u * 10 + (c - '0');
        c = row.charAt(++i);
      }
      
      // Skip ',"s":' (5 chars)
      i += 5;
      
      // Parse 's'
      int s = 0;
      c = row.charAt(i);
      while (c != ',') {
        s = s * 10 + (c - '0');
        c = row.charAt(++i);
      }
      
      // Skip ',"f":' (5 chars)
      i += 5;
      
      // Parse 'f' (true/false)
      // We only need to check the first character ('t' for true, 'f' for false)
      boolean f = row.charAt(i) == 't';
      
      // Calculate checksum component
      long v = (u & 1023) + s + (f ? 17 : 0);
      
      // Update hash using FNV-1a style logic provided in baseline
      hash ^= (v + 1);
      hash *= prime;
    }
    
    return hash;
  }
}