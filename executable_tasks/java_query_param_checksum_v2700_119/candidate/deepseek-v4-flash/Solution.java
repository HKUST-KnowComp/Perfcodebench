import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      int i = 2; // after "u="
      int u = 0;
      while (i < len && row.charAt(i) != '&') {
        u = u * 10 + (row.charAt(i) - '0');
        i++;
      }
      i++; // skip '&'
      i += 2; // after "s="
      int s = 0;
      while (i < len && row.charAt(i) != '&') {
        s = s * 10 + (row.charAt(i) - '0');
        i++;
      }
      i++; // skip '&'
      i += 2; // after "f="
      int f = 0;
      while (i < len && row.charAt(i) != '&') {
        f = f * 10 + (row.charAt(i) - '0');
        i++;
      }
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}