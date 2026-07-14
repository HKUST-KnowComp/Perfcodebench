import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int u = 0, s = 0;
      boolean f = false;
      int i = 5; // skip {"u":
      char c;
      while (true) {
        c = row.charAt(i);
        if (c >= '0' && c <= '9') {
          u = u * 10 + (c - '0');
          i++;
        } else {
          break;
        }
      }
      // skip ,"s":
      i += 5;
      while (true) {
        c = row.charAt(i);
        if (c >= '0' && c <= '9') {
          s = s * 10 + (c - '0');
          i++;
        } else {
          break;
        }
      }
      // skip ,"f":
      i += 5;
      // read true/false
      if (row.charAt(i) == 't') {
        f = true;
      }
      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}