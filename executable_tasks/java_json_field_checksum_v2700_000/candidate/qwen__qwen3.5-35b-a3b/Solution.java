import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int uStart = 5;
      int comma1 = row.indexOf(',', uStart);
      int u = Integer.parseInt(row.substring(uStart, comma1));

      int sStart = comma1 + 5;
      int comma2 = row.indexOf(',', sStart);
      int s = Integer.parseInt(row.substring(sStart, comma2));

      int fStart = comma2 + 5;
      boolean f = row.charAt(fStart) == 't';

      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}