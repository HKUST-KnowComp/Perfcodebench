import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int idx = 5;
      int u = 0;
      while (true) {
        char c = row.charAt(idx);
        if (c == ',') break;
        u = u * 10 + (c - '0');
        idx++;
      }
      idx += 5;
      int s = 0;
      while (true) {
        char c = row.charAt(idx);
        if (c == ',') break;
        s = s * 10 + (c - '0');
        idx++;
      }
      idx += 5;
      boolean f = row.charAt(idx) == 't';
      
      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}