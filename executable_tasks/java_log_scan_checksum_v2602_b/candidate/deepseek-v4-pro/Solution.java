import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      int i = 5; // skip "src=S"
      int src = 0;
      char c;
      while (i < len && (c = row.charAt(i)) != ' ') {
        src = src * 10 + (c - '0');
        i++;
      }
      i++; // skip space
      i += 4; // skip "lat="
      int lat = 0;
      while (i < len && (c = row.charAt(i)) != ' ') {
        lat = lat * 10 + (c - '0');
        i++;
      }
      i++; // skip space
      i += 5; // skip "code="
      int code = 0;
      while (i < len && (c = row.charAt(i)) != ' ') {
        code = code * 10 + (c - '0');
        i++;
      }
      i++; // skip space
      i += 3; // skip "ok="
      boolean ok = row.charAt(i) == 'y';
      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}