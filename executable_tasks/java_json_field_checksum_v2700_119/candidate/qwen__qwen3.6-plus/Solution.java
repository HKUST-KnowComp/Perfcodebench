import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int idx = 0;
      
      // Parse u
      idx = row.indexOf("\"u\":", idx) + 4;
      int u = 0;
      char c;
      while ((c = row.charAt(idx)) >= '0' && c <= '9') {
        u = u * 10 + (c - '0');
        idx++;
      }
      
      // Parse s
      idx = row.indexOf("\"s\":", idx) + 4;
      int s = 0;
      while ((c = row.charAt(idx)) >= '0' && c <= '9') {
        s = s * 10 + (c - '0');
        idx++;
      }
      
      // Parse f
      idx = row.indexOf("\"f\":", idx) + 4;
      boolean f = (row.charAt(idx) == 't');
      
      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}