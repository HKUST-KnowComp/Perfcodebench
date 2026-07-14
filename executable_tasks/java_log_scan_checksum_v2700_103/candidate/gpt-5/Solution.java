import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final int size = rows.size();
    for (int r = 0; r < size; r++) {
      String row = rows.get(r);
      int n = row.length();
      int i = 5; // skip "src=S"

      // parse src
      int src = 0;
      while (i < n) {
        char ch = row.charAt(i);
        if (ch < '0' || ch > '9') break;
        src = src * 10 + (ch - '0');
        i++;
      }

      // skip " lat=" (space + 'l''a''t''=')
      i += 5;

      // parse lat
      int lat = 0;
      while (i < n) {
        char ch = row.charAt(i);
        if (ch < '0' || ch > '9') break;
        lat = lat * 10 + (ch - '0');
        i++;
      }

      // skip " code=" (space + 'c''o''d''e''=')
      i += 6;

      // parse code
      int code = 0;
      while (i < n) {
        char ch = row.charAt(i);
        if (ch < '0' || ch > '9') break;
        code = code * 10 + (ch - '0');
        i++;
      }

      // skip " ok=" (space + 'o''k''=')
      i += 4;

      boolean ok = row.charAt(i) == 'y';

      int sum = (src & 255) + lat + code + (ok ? 31 : 0);
      long v = (long) sum + 1L;
      hash ^= v;
      hash *= 1099511628211L;
    }
    return hash;
  }
}