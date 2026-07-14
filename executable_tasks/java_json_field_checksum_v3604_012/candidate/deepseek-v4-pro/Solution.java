import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      int pos = 5; // skip {"u":
      // parse u
      int u = 0;
      char c;
      while (pos < len && (c = row.charAt(pos)) >= '0' && c <= '9') {
        u = u * 10 + (c - '0');
        pos++;
      }
      // skip ,"s":
      pos += 5;
      // parse s
      int s = 0;
      while (pos < len && (c = row.charAt(pos)) >= '0' && c <= '9') {
        s = s * 10 + (c - '0');
        pos++;
      }
      // skip ,"f":
      pos += 5;
      // determine f
      boolean f = (row.charAt(pos) == 't');
      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}