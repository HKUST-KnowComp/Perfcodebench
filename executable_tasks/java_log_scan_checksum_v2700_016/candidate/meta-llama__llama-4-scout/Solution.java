import java.util.List;
public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int src = row.charAt(4) - '0';
      int lat = 0, code = 0;
      boolean ok;
      int i = 9;
      while (row.charAt(i) != ' ') {
        lat = lat * 10 + (row.charAt(i++) - '0');
      }
      i++;
      while (row.charAt(i) != ' ') {
        code = code * 10 + (row.charAt(i++) - '0');
      }
      i += 4;
      ok = row.charAt(i) == 'y';
      long v = src + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}