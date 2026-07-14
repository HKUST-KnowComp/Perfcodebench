import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      char[] chars = row.toCharArray();
      int len = chars.length;
      
      int u = 0;
      int s = 0;
      boolean f = false;
      
      int i = 0;
      // Skip initial {\"u\":
      while (i < len && chars[i] != 'u') i++;
      i += 5; // skip u\":
      while (i < len && chars[i] >= '0' && chars[i] <= '9') {
        u = u * 10 + (chars[i] - '0');
        i++;
      }
      
      // Skip ,\"s\":
      while (i < len && chars[i] != 's') i++;
      i += 5; // skip s\":
      while (i < len && chars[i] >= '0' && chars[i] <= '9') {
        s = s * 10 + (chars[i] - '0');
        i++;
      }
      
      // Skip ,\"f\":
      while (i < len && chars[i] != 'f') i++;
      i += 5; // skip f\":
      if (chars[i] == 't') {
        f = true;
        i += 4;
      } else {
        f = false;
        i += 5;
      }

      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}