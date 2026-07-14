import java.util.*;

public class Solution {
  private static int parseIntFrom(String s, int start, int end) {
    int val = 0;
    while (start < end) {
      val = val * 10 + (s.charAt(start) - '0');
      start++;
    }
    return val;
  }

  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int colon1 = row.indexOf(':');
      int comma1 = row.indexOf(',', colon1);
      int u = parseIntFrom(row, colon1 + 1, comma1);

      int colon2 = row.indexOf(':', comma1);
      int comma2 = row.indexOf(',', colon2);
      int s = parseIntFrom(row, colon2 + 1, comma2);

      int colon3 = row.indexOf(':', comma2);
      boolean f = row.charAt(colon3 + 1) == 't';

      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}