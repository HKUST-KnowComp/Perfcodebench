import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      int i = 5; // skip {"u":
      int u = 0;
      while (i < len && row.charAt(i) != ',') {
        u = u * 10 + (row.charAt(i) - '0');
        i++;
      }
      i += 4; // skip ,"s":
      int s = 0;
      while (i < len && row.charAt(i) != ',') {
        s = s * 10 + (row.charAt(i) - '0');
        i++;
      }
      i += 4; // skip ,"f":
      boolean f = (row.charAt(i) == 't');
      // skip remaining characters (true/false and rest of string) – not needed
      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}