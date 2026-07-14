import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int comma1 = row.indexOf(',', 5);
      int colon2 = row.indexOf(':', comma1 + 1);
      int comma2 = row.indexOf(',', colon2 + 1);
      int colon3 = row.indexOf(':', comma2 + 1);
      int comma3 = row.indexOf(',', colon3 + 1);

      int u = Integer.parseInt(row.substring(5, comma1));
      int s = Integer.parseInt(row.substring(colon2 + 1, comma2));
      boolean f = row.substring(colon3 + 1, comma3).equals("true");

      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}