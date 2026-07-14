import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();

      // u
      int p = 6; // skip {"u":
      int u = 0;
      while (p < len && row.charAt(p) != ',') {
        u = u * 10 + (row.charAt(p++) - '0');
      }

      // s
      p += 6; // skip ,"s":
      int s = 0;
      while (p < len && row.charAt(p) != ',') {
        s = s * 10 + (row.charAt(p++) - '0');
      }

      // f
      p += 6; // skip ,"f":
      int f = (row.charAt(p) == 't') ? 17 : 0;

      long v = (u & 1023) + s + f;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}