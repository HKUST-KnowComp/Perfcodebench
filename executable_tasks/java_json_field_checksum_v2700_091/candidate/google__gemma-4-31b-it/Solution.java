import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      int pos = 0;
      
      // Expect "{" 
      pos++; 
      
      // Find "u":
      pos += 4; // "u":
      int u = 0;
      while (pos < len && row.charAt(pos) >= '0' && row.charAt(pos) <= '9') {
        u = u * 10 + (row.charAt(pos++) - '0');
      }
      
      // Find "s":
      pos += 4; // ,"s":
      int s = 0;
      while (pos < len && row.charAt(pos) >= '0' && row.charAt(pos) <= '9') {
        s = s * 10 + (row.charAt(pos++) - '0');
      }
      
      // Find "f":
      pos += 4; // ,"f":
      boolean f = false;
      if (row.charAt(pos) == 't') {
        f = true;
        pos += 4; // true
      } else {
        pos += 5; // false
      }
      
      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}