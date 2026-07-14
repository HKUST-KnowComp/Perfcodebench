import java.util.List;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final int n = rows.size();
    for (int r = 0; r < n; r++) {
      String s = rows.get(r);
      int len = s.length();
      int i = 5; // skip "src=S"

      // parse src digits
      int src = 0;
      while (i < len) {
        char ch = s.charAt(i);
        int d = ch - '0';
        if ((d | (9 - d)) < 0) break; // fast range check for 0..9
        src = src * 10 + d;
        i++;
      }

      i++;       // skip space
      i += 4;    // skip "lat="

      // parse lat digits
      int lat = 0;
      while (i < len) {
        char ch = s.charAt(i);
        int d = ch - '0';
        if ((d | (9 - d)) < 0) break;
        lat = lat * 10 + d;
        i++;
      }

      i++;       // skip space
      i += 5;    // skip "code="

      // parse code digits
      int code = 0;
      while (i < len) {
        char ch = s.charAt(i);
        int d = ch - '0';
        if ((d | (9 - d)) < 0) break;
        code = code * 10 + d;
        i++;
      }

      i++;       // skip space
      i += 3;    // skip "ok="

      boolean ok = s.charAt(i) == 'y'; // 'yes' or 'no'

      int sum = (src & 255) + lat + code + (ok ? 31 : 0);
      long v = (long) sum; // preserve int overflow semantics before widening
      hash ^= (v + 1L);
      hash *= 1099511628211L;
    }
    return hash;
  }
}