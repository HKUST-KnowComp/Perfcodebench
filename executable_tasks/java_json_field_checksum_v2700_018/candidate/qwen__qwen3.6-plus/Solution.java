import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int i = 5; // skip {"u":
      int u = 0;
      char c;
      while ((c = row.charAt(i)) >= '0' && c <= '9') {
        u = u * 10 + (c - '0');
        i++;
      }
      i += 5; // skip ,"s":
      int s = 0;
      while ((c = row.charAt(i)) >= '0' && c <= '9') {
        s = s * 10 + (c - '0');
        i++;
      }
      i += 5; // skip ,"f":
      boolean f = row.charAt(i) == 't';
      i += f ? 4 : 5;
      i++; // skip ,
      
      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}