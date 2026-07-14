import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      int pos = 1; // after '{'
      // skip "u":
      pos += 4;
      int u = 0;
      while (true) {
        char c = row.charAt(pos);
        if (c == ',') { pos++; break; }
        u = u * 10 + (c - '0');
        pos++;
      }
      // skip "s":
      pos += 4;
      int s = 0;
      while (true) {
        char c = row.charAt(pos);
        if (c == ',') { pos++; break; }
        s = s * 10 + (c - '0');
        pos++;
      }
      // skip "f":
      pos += 4;
      boolean f = row.charAt(pos) == 't';
      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}