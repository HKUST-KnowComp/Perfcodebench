import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      int i = 2;
      int u = 0;
      while (i < len) {
        char c = row.charAt(i);
        if (c < '0' || c > '9') break;
        u = u * 10 + (c - '0');
        i++;
      }
      i += 7;
      int s = 0;
      while (i < len) {
        char c = row.charAt(i);
        if (c < '0' || c > '9') break;
        s = s * 10 + (c - '0');
        i++;
      }
      i += 6;
      int f = 0;
      while (i < len) {
        char c = row.charAt(i);
        if (c < '0' || c > '9') break;
        f = f * 10 + (c - '0');
        i++;
      }
      
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}