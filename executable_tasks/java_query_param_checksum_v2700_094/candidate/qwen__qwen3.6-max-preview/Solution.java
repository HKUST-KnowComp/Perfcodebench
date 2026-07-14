import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      int idx = row.indexOf('=') + 1;
      
      int sign = 1;
      if (idx < len && row.charAt(idx) == '-') { sign = -1; idx++; }
      int u = 0;
      while (idx < len) {
        char c = row.charAt(idx);
        if (c < '0' || c > '9') break;
        u = u * 10 + (c - '0');
        idx++;
      }
      u *= sign;
      
      idx = row.indexOf('=', idx) + 1;
      sign = 1;
      if (idx < len && row.charAt(idx) == '-') { sign = -1; idx++; }
      int s = 0;
      while (idx < len) {
        char c = row.charAt(idx);
        if (c < '0' || c > '9') break;
        s = s * 10 + (c - '0');
        idx++;
      }
      s *= sign;
      
      idx = row.indexOf('=', idx) + 1;
      sign = 1;
      if (idx < len && row.charAt(idx) == '-') { sign = -1; idx++; }
      int f = 0;
      while (idx < len) {
        char c = row.charAt(idx);
        if (c < '0' || c > '9') break;
        f = f * 10 + (c - '0');
        idx++;
      }
      f *= sign;
      
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}