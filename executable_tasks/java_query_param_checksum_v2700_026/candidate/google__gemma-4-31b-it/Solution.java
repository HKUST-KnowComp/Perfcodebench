import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      
      // Extract u: starts at index 2, ends at first '&'
      int u = 0;
      int i = 2;
      while (i < len && row.charAt(i) != '&') {
        u = u * 10 + (row.charAt(i++) - '0');
      }
      
      // Move to start of s: skip '&' and "s=" (3 chars)
      i += 3;
      int s = 0;
      while (i < len && row.charAt(i) != '&') {
        s = s * 10 + (row.charAt(i++) - '0');
      }
      
      // Move to start of f: skip '&' and "f=" (2 chars)
      i += 2;
      int f = 0;
      while (i < len) {
        f = f * 10 + (row.charAt(i++) - '0');
      }
      
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}