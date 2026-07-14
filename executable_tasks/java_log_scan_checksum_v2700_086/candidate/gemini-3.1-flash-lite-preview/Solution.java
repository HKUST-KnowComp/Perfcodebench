import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      // Format: src=S(\d+) lat=(\d+) code=(\d+) ok=(yes|no)
      int src = 0;
      int lat = 0;
      int code = 0;
      boolean ok = false;

      int i = 5; // skip "src=S"
      while (row.charAt(i) != ' ') {
        src = src * 10 + (row.charAt(i) - '0');
        i++;
      }
      i += 5; // skip " lat="
      while (row.charAt(i) != ' ') {
        lat = lat * 10 + (row.charAt(i) - '0');
        i++;
      }
      i += 6; // skip " code="
      while (row.charAt(i) != ' ') {
        code = code * 10 + (row.charAt(i) - '0');
        i++;
      }
      i += 4; // skip " ok="
      if (row.charAt(i) == 'y') {
        ok = true;
      }

      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}