import java.util.*;

public class Solution {
  /**
   * Optimizes the extraction of fields 'u', 's', and 'f' from a fixed-format JSON string.
   * The expected format is: {"u":\d+,"s":\d+,"f":true|false,"tag":"x\d+"}
   */
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final long prime = 1099511628211L;

    for (String row : rows) {
      int len = row.length();
      if (len == 0) continue;

      // Skip '{"u":' (5 chars)
      int i = 5;
      
      // Parse 'u'
      int u = 0;
      char c = row.charAt(i);
      while (c >= '0' && c <= '9') {
        u = u * 10 + (c - '0');
        i++;
        c = row.charAt(i);
      }

      // Skip ',"s":' (5 chars)
      i += 5;
      
      // Parse 's'
      int s = 0;
      c = row.charAt(i);
      while (c >= '0' && c <= '9') {
        s = s * 10 + (c - '0');
        i++;
        c = row.charAt(i);
      }

      // Skip ',"f":' (5 chars)
      i += 5;
      
      // Parse 'f' (true or false)
      boolean f;
      if (row.charAt(i) == 't') {
        f = true;
        // i += 4; // skip 'true'
      } else {
        f = false;
        // i += 5; // skip 'false'
      }

      // Calculate value and update FNV-1a style hash
      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= prime;
    }
    return hash;
  }
}