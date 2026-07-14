import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final int n = rows.size();
    for (int i = 0; i < n; i++) {
      String row = rows.get(i);
      int len = row.length();

      int p = 2; // after first key prefix, baseline assumes first field starts at index 2
      int u = 0;
      while (p < len) {
        char c = row.charAt(p);
        if (c == '&') break;
        u = u * 10 + (c - '0');
        p++;
      }

      p += 6; // skip '&' and second key prefix, matching baseline substring(6)
      int s = 0;
      while (p < len) {
        char c = row.charAt(p);
        if (c == '&') break;
        s = s * 10 + (c - '0');
        p++;
      }

      p += 5; // skip '&' and third key prefix, matching baseline substring(5)
      int f = 0;
      while (p < len) {
        char c = row.charAt(p);
        f = f * 10 + (c - '0');
        p++;
      }

      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}
