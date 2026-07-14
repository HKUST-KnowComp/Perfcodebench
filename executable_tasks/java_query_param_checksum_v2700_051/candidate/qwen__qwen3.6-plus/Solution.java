import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    int n = rows.size();
    for (int i = 0; i < n; i++) {
      String row = rows.get(i);
      int len = row.length();
      int idx = 0;

      // Parse u
      while (idx < len && row.charAt(idx) != '=') idx++;
      idx++;
      int u = 0;
      while (idx < len) {
        char c = row.charAt(idx);
        if (c < '0' || c > '9') { idx++; break; }
        u = u * 10 + (c - '0');
        idx++;
      }

      // Parse s
      while (idx < len && row.charAt(idx) != '=') idx++;
      idx++;
      int s = 0;
      while (idx < len) {
        char c = row.charAt(idx);
        if (c < '0' || c > '9') { idx++; break; }
        s = s * 10 + (c - '0');
        idx++;
      }

      // Parse f
      while (idx < len && row.charAt(idx) != '=') idx++;
      idx++;
      int f = 0;
      while (idx < len) {
        char c = row.charAt(idx);
        if (c < '0' || c > '9') break;
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