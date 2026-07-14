import java.util.List;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int i = 5;
      int src = 0;
      char c;
      while ((c = row.charAt(i++)) != ' ') {
        src = src * 10 + (c - '0');
      }
      i += 4;
      int lat = 0;
      while ((c = row.charAt(i++)) != ' ') {
        lat = lat * 10 + (c - '0');
      }
      i += 5;
      int code = 0;
      while ((c = row.charAt(i++)) != ' ') {
        code = code * 10 + (c - '0');
      }
      i += 3;
      boolean ok = row.charAt(i) == 'y';

      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}