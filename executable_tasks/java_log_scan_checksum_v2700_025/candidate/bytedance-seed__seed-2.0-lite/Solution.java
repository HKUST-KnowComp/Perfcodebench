import java.util.List;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int i1 = row.indexOf(' ');
      int i2 = row.indexOf(' ', i1 + 1);
      int i3 = row.indexOf(' ', i2 + 1);
      
      int src = Integer.parseInt(row, 5, i1, 10);
      int lat = Integer.parseInt(row, i1 + 5, i2, 10);
      int code = Integer.parseInt(row, i2 + 6, i3, 10);
      boolean ok = row.charAt(i3 + 4) == 'y';
      
      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}