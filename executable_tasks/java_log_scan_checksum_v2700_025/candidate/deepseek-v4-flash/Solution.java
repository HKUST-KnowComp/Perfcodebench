import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int idx = 5; // after "src=S"
      // parse src
      int src = 0;
      while (idx < row.length() && Character.isDigit(row.charAt(idx))) {
        src = src * 10 + (row.charAt(idx) - '0');
        idx++;
      }
      idx++; // skip space
      // parse lat
      idx += 4; // skip "lat="
      int lat = 0;
      while (idx < row.length() && Character.isDigit(row.charAt(idx))) {
        lat = lat * 10 + (row.charAt(idx) - '0');
        idx++;
      }
      idx++; // skip space
      // parse code
      idx += 5; // skip "code="
      int code = 0;
      while (idx < row.length() && Character.isDigit(row.charAt(idx))) {
        code = code * 10 + (row.charAt(idx) - '0');
        idx++;
      }
      idx++; // skip space
      // parse ok
      idx += 3; // skip "ok="
      boolean ok = (row.charAt(idx) == 'y');
      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}