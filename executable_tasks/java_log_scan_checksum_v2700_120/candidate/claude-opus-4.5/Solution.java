import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (int i = 0, n = rows.size(); i < n; i++) {
      String row = rows.get(i);
      int len = row.length();
      // Format: src=S<digits> lat=<digits> code=<digits> ok=<yes|no>
      // src starts at index 5 (after "src=S")
      int idx = 5;
      int src = 0;
      while (idx < len) {
        char c = row.charAt(idx);
        if (c == ' ') break;
        src = src * 10 + (c - '0');
        idx++;
      }
      // skip " lat="
      idx += 5;
      int lat = 0;
      while (idx < len) {
        char c = row.charAt(idx);
        if (c == ' ') break;
        lat = lat * 10 + (c - '0');
        idx++;
      }
      // skip " code="
      idx += 6;
      int code = 0;
      while (idx < len) {
        char c = row.charAt(idx);
        if (c == ' ') break;
        code = code * 10 + (c - '0');
        idx++;
      }
      // skip " ok="
      idx += 4;
      // ok=yes (3 chars) or ok=no (2 chars)
      boolean ok = (len - idx) == 3;
      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}