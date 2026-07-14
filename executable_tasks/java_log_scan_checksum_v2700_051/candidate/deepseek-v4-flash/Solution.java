import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int i = 5; // skip "src=S"
      int src = 0;
      while (row.charAt(i) != ' ') {
        src = src * 10 + (row.charAt(i) - '0');
        i++;
      }
      i++; // skip space
      // skip "lat="
      i += 4;
      int lat = 0;
      while (row.charAt(i) != ' ') {
        lat = lat * 10 + (row.charAt(i) - '0');
        i++;
      }
      i++; // skip space
      // skip "code="
      i += 5;
      int code = 0;
      while (row.charAt(i) != ' ') {
        code = code * 10 + (row.charAt(i) - '0');
        i++;
      }
      i++; // skip space
      // skip "ok="
      i += 3;
      boolean ok = row.charAt(i) == 'y';
      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}