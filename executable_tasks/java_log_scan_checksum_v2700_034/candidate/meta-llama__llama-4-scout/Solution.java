import java.util.List;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int src = row.charAt(4) - '0';
      int lat = row.charAt(9) - '0';
      int code = row.charAt(14) - '0';
      boolean ok = row.charAt(19) == 'y';
      int v = src + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}