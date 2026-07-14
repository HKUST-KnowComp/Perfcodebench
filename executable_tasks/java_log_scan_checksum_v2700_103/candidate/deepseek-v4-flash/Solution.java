import java.util.*;
import java.util.regex.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      int i = 5; // after "src=S"
      int src = 0;
      while (i < len && row.charAt(i) >= '0' && row.charAt(i) <= '9') {
        src = src * 10 + (row.charAt(i) - '0');
        i++;
      }
      i++; // skip space
      i += 4; // skip "lat="
      int lat = 0;
      while (i < len && row.charAt(i) >= '0' && row.charAt(i) <= '9') {
        lat = lat * 10 + (row.charAt(i) - '0');
        i++;
      }
      i++; // skip space
      i += 5; // skip "code="
      int code = 0;
      while (i < len && row.charAt(i) >= '0' && row.charAt(i) <= '9') {
        code = code * 10 + (row.charAt(i) - '0');
        i++;
      }
      i++; // skip space before "ok="
      i += 3; // skip "ok="
      int okAdd = (row.charAt(i) == 'y') ? 31 : 0;
      long v = (src & 255) + lat + code + okAdd;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}