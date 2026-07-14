import java.util.*;

public class Solution {
  private static final long MULT = 1099511628211L;

  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      // Parse the fixed format line:
      // src=S<num> lat=<num> code=<num> ok=yes|no
      int len = row.length();
      int i = 5; // skip "src=S"
      // parse src integer
      int src = 0;
      while (i < len) {
        char c = row.charAt(i);
        if (c == ' ') break;
        src = src * 10 + (c - '0');
        i++;
      }
      // after src number, we are at space; skip it and "lat="
      i += 5; // skip space + "lat=" (4 chars)
      int lat = 0;
      while (i < len) {
        char c = row.charAt(i);
        if (c == ' ') break;
        lat = lat * 10 + (c - '0');
        i++;
      }
      // skip space and "code="
      i += 6; // space + "code=" (5 chars)
      int code = 0;
      while (i < len) {
        char c = row.charAt(i);
        if (c == ' ') break;
        code = code * 10 + (c - '0');
        i++;
      }
      // skip space and "ok="
      i += 4; // space + "ok=" (3 chars)
      boolean ok = row.charAt(i) == 'y'; // 'y' for "yes", 'n' for "no"
      // compute value and update hash
      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= MULT;
    }
    return hash;
  }
}