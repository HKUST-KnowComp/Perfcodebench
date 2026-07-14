import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int i = 1; // skip initial '{'
      int len = row.length();

      // skip to first colon, then parse u
      while (row.charAt(i) != ':') i++;
      i++; // skip ':'
      int u = 0;
      while (i < len && row.charAt(i) >= '0' && row.charAt(i) <= '9') {
        u = u * 10 + (row.charAt(i) - '0');
        i++;
      }

      // skip comma and field name to second colon
      i++; // skip ','
      while (row.charAt(i) != ':') i++;
      i++; // skip ':'
      int s = 0;
      while (i < len && row.charAt(i) >= '0' && row.charAt(i) <= '9') {
        s = s * 10 + (row.charAt(i) - '0');
        i++;
      }

      // skip comma and field name to third colon
      i++; // skip ','
      while (row.charAt(i) != ':') i++;
      i++; // skip ':'
      boolean f = (row.charAt(i) == 't'); // 't' for true, anything else false

      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}