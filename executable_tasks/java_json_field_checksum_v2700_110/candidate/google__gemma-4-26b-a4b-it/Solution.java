import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int u = 0;
      int s = 0;
      boolean f = false;
      
      int i = 0;
      int len = row.length();

      // Skip to "u":
      while (i < len && row.charAt(i) != 'u') i++;
      i += 4; // skip 'u":('n
      while (i < len && row.charAt(i) >= '0' && row.charAt(i) <= '9') {
        u = u * 10 + (row.charAt(i) - '0');
        i++;
      }

      // Skip to "s":
      while (i < len && row.charAt(i) != 's') i++;
      i += 4; // skip 's":('n
      while (i < len && row.charAt(i) >= '0' && row.charAt(i) <= '9') {
        s = s * 10 + (row.charAt(i) - '0');
        i++;
      }

      // Skip to "f":
      while (i < len && row.charAt(i) != 'f') i++;
      i += 4; // skip 'f":('n
      if (row.charAt(i) == 't') {
        f = true;
        i += 4;
      } else {
        f = false;
        i += 5;
      }

      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}