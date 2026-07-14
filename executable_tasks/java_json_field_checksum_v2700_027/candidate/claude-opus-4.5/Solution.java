import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (int i = 0, n = rows.size(); i < n; i++) {
      String row = rows.get(i);
      int len = row.length();
      
      // Parse u: find first ':' after position 0, then parse digits
      int idx = 5; // skip {"u":
      int u = 0;
      char c;
      while ((c = row.charAt(idx)) >= '0' && c <= '9') {
        u = u * 10 + (c - '0');
        idx++;
      }
      
      // Parse s: skip ","s":", then parse digits
      idx += 5; // skip ,"s":
      int s = 0;
      while ((c = row.charAt(idx)) >= '0' && c <= '9') {
        s = s * 10 + (c - '0');
        idx++;
      }
      
      // Parse f: skip ","f":", check first char (t for true, f for false)
      idx += 5; // skip ,"f":
      boolean f = row.charAt(idx) == 't';
      
      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}