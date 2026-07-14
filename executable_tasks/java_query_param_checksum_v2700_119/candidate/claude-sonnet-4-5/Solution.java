import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      
      // Parse u=<digits>&
      int pos = 2; // skip "u="
      int u = 0;
      while (pos < len && row.charAt(pos) != '&') {
        u = u * 10 + (row.charAt(pos) - '0');
        pos++;
      }
      pos++; // skip '&'
      
      // Parse status=<digits>&
      pos += 7; // skip "status="
      int s = 0;
      while (pos < len && row.charAt(pos) != '&') {
        s = s * 10 + (row.charAt(pos) - '0');
        pos++;
      }
      pos++; // skip '&'
      
      // Parse flag=<digits>
      pos += 5; // skip "flag="
      int f = 0;
      while (pos < len) {
        f = f * 10 + (row.charAt(pos) - '0');
        pos++;
      }
      
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}