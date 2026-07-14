import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int c1 = row.indexOf(',');
      int c2 = row.indexOf(',', c1 + 1);
      int c3 = row.indexOf(',', c2 + 1);
      int c4 = row.indexOf(',', c3 + 1);

      int b = Integer.parseInt(row.substring(c2 + 1, c3));
      int c = Integer.parseInt(row.substring(c3 + 1, c4));
      boolean hot = row.charAt(c4 + 1) == 'h' && row.charAt(c4 + 2) == 'o' && row.charAt(c4 + 3) == 't';

      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}