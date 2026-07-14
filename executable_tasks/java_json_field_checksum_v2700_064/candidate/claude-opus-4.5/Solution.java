import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (int i = 0, n = rows.size(); i < n; i++) {
      String row = rows.get(i);
      int len = row.length();
      
      // Parse u: starts at index 5 after {"u":
      int idx = 5;
      int u = 0;
      char c;
      while ((c = row.charAt(idx)) != ',') {
        u = u * 10 + (c - '0');
        idx++;
      }
      
      // Parse s: skip ,"s": (5 chars)
      idx += 5;
      int s = 0;
      while ((c = row.charAt(idx)) != ',') {
        s = s * 10 + (c - '0');
        idx++;
      }
      
      // Parse f: skip ,"f": (5 chars), check if 't' for true
      idx += 5;
      boolean f = row.charAt(idx) == 't';
      
      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}