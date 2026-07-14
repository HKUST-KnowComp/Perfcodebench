import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      int idx = 6; // skip '{"u":'
      
      // Parse u
      int u = 0;
      while (idx < len && row.charAt(idx) >= '0' && row.charAt(idx) <= '9') {
        u = u * 10 + (row.charAt(idx) - '0');
        idx++;
      }
      
      idx += 6; // skip ',"s":'
      
      // Parse s
      int s = 0;
      while (idx < len && row.charAt(idx) >= '0' && row.charAt(idx) <= '9') {
        s = s * 10 + (row.charAt(idx) - '0');
        idx++;
      }
      
      idx += 6; // skip ',"f":'
      
      // Parse f
      boolean f = row.charAt(idx) == 't';
      
      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}
