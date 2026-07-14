import java.util.*;

public class Solution {
  /**
   * Optimizes the processing of JSON-like rows by manually parsing fields.
   * The expected format is: {"u":\d+,"s":\d+,"f":(true|false),"tag":"x\d+"}
   */
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final long prime = 1099511628211L;

    for (String row : rows) {
      // Field "u" starts at index 5
      int i = 5;
      int u = 0;
      char c = row.charAt(i);
      while (c != ',') {
        u = u * 10 + (c - '0');
        c = row.charAt(++i);
      }

      // Skip ",\"s\":" which is 5 characters
      i += 6;
      int s = 0;
      c = row.charAt(i);
      while (c != ',') {
        s = s * 10 + (c - '0');
        c = row.charAt(++i);
      }

      // Skip ",\"f\":" which is 5 characters
      i += 5;
      // Check if the boolean starts with 't' (true) or 'f' (false)
      boolean f = row.charAt(i) == 't';

      // Calculate the value based on the extracted fields
      long v = (u & 1023) + s + (f ? 17 : 0);
      
      // FNV-1a style hash update
      hash ^= (v + 1);
      hash *= prime;
    }
    
    return hash;
  }
}