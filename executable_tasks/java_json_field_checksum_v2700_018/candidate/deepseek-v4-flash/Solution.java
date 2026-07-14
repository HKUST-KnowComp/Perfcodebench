import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int i = 0;
      // skip '{'
      i++;
      // skip "u":
      i += 4;
      // parse u digits
      int u = 0;
      while (row.charAt(i) != ',') {
        u = u * 10 + (row.charAt(i) - '0');
        i++;
      }
      i++; // skip ','
      // skip "s":
      i += 4;
      // parse s digits
      int s = 0;
      while (row.charAt(i) != ',') {
        s = s * 10 + (row.charAt(i) - '0');
        i++;
      }
      i++; // skip ','
      // skip "f":
      i += 4;
      // parse f boolean
      boolean f = row.charAt(i) == 't';
      i += f ? 4 : 5; // skip the word
      // compute contribution
      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}