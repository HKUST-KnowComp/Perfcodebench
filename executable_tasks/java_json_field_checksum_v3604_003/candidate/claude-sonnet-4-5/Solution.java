import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      int i = 6; // skip '{"u":'
      
      // Parse u
      int u = 0;
      while (i < len && row.charAt(i) >= '0' && row.charAt(i) <= '9') {
        u = u * 10 + (row.charAt(i) - '0');
        i++;
      }
      
      i += 6; // skip ',"s":'
      
      // Parse s
      int s = 0;
      while (i < len && row.charAt(i) >= '0' && row.charAt(i) <= '9') {
        s = s * 10 + (row.charAt(i) - '0');
        i++;
      }
      
      i += 6; // skip ',"f":'
      
      // Parse f (true/false)
      boolean f = row.charAt(i) == 't';
      
      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}
