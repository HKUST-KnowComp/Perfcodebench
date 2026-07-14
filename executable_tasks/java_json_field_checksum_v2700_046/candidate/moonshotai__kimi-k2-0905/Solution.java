import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      char[] cs = row.toCharArray();
      int u = 0, s = 0;
      boolean f = false;
      int i = 5; // skip {"u":
      while (cs[i] != ',') {
        u = u * 10 + (cs[i++] - '0');
      }
      i += 6; // skip ,"s":
      while (cs[i] != ',') {
        s = s * 10 + (cs[i++] - '0');
      }
      i += 6; // skip ,"f":
      f = cs[i] == 't';
      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}