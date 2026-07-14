import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      // assume format: {"u":123,"s":456,"f":true,"tag":"x789"}
      int i = 5; // after {"u":
      int u = 0;
      while (row.charAt(i) >= '0' && row.charAt(i) <= '9') {
        u = u * 10 + (row.charAt(i) - '0');
        i++;
      }
      i++; // skip comma
      // skip "s":
      i += 4;
      int s = 0;
      while (row.charAt(i) >= '0' && row.charAt(i) <= '9') {
        s = s * 10 + (row.charAt(i) - '0');
        i++;
      }
      i++; // skip comma
      // skip "f":
      i += 4;
      boolean f = (row.charAt(i) == 't');
      if (f) i += 4; else i += 5;
      // i now points after 'true,' or 'false,'
      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}