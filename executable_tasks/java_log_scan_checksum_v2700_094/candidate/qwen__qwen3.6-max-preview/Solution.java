import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      int i = 5;
      int src = 0;
      while (i < len) {
        char c = row.charAt(i);
        if (c < '0' || c > '9') break;
        src = src * 10 + (c - '0');
        i++;
      }
      i += 5;
      int lat = 0;
      while (i < len) {
        char c = row.charAt(i);
        if (c < '0' || c > '9') break;
        lat = lat * 10 + (c - '0');
        i++;
      }
      i += 6;
      int code = 0;
      while (i < len) {
        char c = row.charAt(i);
        if (c < '0' || c > '9') break;
        code = code * 10 + (c - '0');
        i++;
      }
      boolean ok = row.charAt(len - 3) == 'y';
      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}