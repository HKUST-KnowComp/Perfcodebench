import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int u = 0;
      int s = 0;
      boolean f = false;
      
      int len = row.length();
      int i = 6; // skip '{"u":'
      
      // Parse u
      while (i < len && row.charAt(i) >= '0' && row.charAt(i) <= '9') {
        u = u * 10 + (row.charAt(i) - '0');
        i++;
      }
      
      i += 6; // skip ',"s":'
      
      // Parse s
      while (i < len && row.charAt(i) >= '0' && row.charAt(i) <= '9') {
        s = s * 10 + (row.charAt(i) - '0');
        i++;
      }
      
      i += 6; // skip ',"f":'
      
      // Parse f
      f = row.charAt(i) == 't';
      
      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}