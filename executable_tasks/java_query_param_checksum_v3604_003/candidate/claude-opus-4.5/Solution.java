import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (int i = 0, n = rows.size(); i < n; i++) {
      String row = rows.get(i);
      int len = row.length();
      int idx = 2;
      int u = 0;
      while (idx < len) {
        char c = row.charAt(idx);
        if (c == '&') break;
        u = u * 10 + (c - '0');
        idx++;
      }
      idx += 7;
      int s = 0;
      while (idx < len) {
        char c = row.charAt(idx);
        if (c == '&') break;
        s = s * 10 + (c - '0');
        idx++;
      }
      idx += 6;
      int f = 0;
      while (idx < len) {
        char c = row.charAt(idx);
        f = f * 10 + (c - '0');
        idx++;
      }
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}
