import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      int idx = 0;
      
      int u = 0, s = 0, f = 0;
      boolean neg;
      
      // Parse u
      while (idx < len && row.charAt(idx) != '=') idx++;
      idx++;
      neg = false;
      if (idx < len && row.charAt(idx) == '-') { neg = true; idx++; }
      while (idx < len) {
        char c = row.charAt(idx);
        if (c < '0' || c > '9') break;
        u = u * 10 + (c - '0');
        idx++;
      }
      if (neg) u = -u;
      
      // Parse s
      while (idx < len && row.charAt(idx) != '=') idx++;
      idx++;
      neg = false;
      if (idx < len && row.charAt(idx) == '-') { neg = true; idx++; }
      while (idx < len) {
        char c = row.charAt(idx);
        if (c < '0' || c > '9') break;
        s = s * 10 + (c - '0');
        idx++;
      }
      if (neg) s = -s;
      
      // Parse f
      while (idx < len && row.charAt(idx) != '=') idx++;
      idx++;
      neg = false;
      if (idx < len && row.charAt(idx) == '-') { neg = true; idx++; }
      while (idx < len) {
        char c = row.charAt(idx);
        if (c < '0' || c > '9') break;
        f = f * 10 + (c - '0');
        idx++;
      }
      if (neg) f = -f;
      
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}