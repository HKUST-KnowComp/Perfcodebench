import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      
      // Parse u=<digits>&
      int idx = 2; // skip "u="
      int u = 0;
      while (idx < len && row.charAt(idx) != '&') {
        u = u * 10 + (row.charAt(idx) - '0');
        idx++;
      }
      idx++; // skip '&'
      
      // Parse status=<digits>&
      idx += 7; // skip "status="
      int s = 0;
      while (idx < len && row.charAt(idx) != '&') {
        s = s * 10 + (row.charAt(idx) - '0');
        idx++;
      }
      idx++; // skip '&'
      
      // Parse flag=<digits>
      idx += 5; // skip "flag="
      int f = 0;
      while (idx < len) {
        f = f * 10 + (row.charAt(idx) - '0');
        idx++;
      }
      
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}
