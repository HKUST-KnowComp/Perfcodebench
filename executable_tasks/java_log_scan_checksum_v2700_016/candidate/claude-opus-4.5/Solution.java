import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (int i = 0, n = rows.size(); i < n; i++) {
      String row = rows.get(i);
      int len = row.length();
      int idx = 5; // skip "src=S"
      int src = 0;
      char c;
      while ((c = row.charAt(idx)) != ' ') {
        src = src * 10 + (c - '0');
        idx++;
      }
      idx += 5; // skip " lat="
      int lat = 0;
      while ((c = row.charAt(idx)) != ' ') {
        lat = lat * 10 + (c - '0');
        idx++;
      }
      idx += 6; // skip " code="
      int code = 0;
      while ((c = row.charAt(idx)) != ' ') {
        code = code * 10 + (c - '0');
        idx++;
      }
      // ok=yes or ok=no: check character at len-2
      // "ok=yes" ends with 's' at len-1, 'e' at len-2
      // "ok=no" ends with 'o' at len-1, 'n' at len-2
      boolean ok = row.charAt(len - 2) == 'e';
      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}
