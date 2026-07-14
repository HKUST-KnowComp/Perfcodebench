import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int j = 5;
      int src = 0;
      while (row.charAt(j) != ' ') {
        src = src * 10 + (row.charAt(j++) - '0');
      }
      j += 5;
      int lat = 0;
      while (row.charAt(j) != ' ') {
        lat = lat * 10 + (row.charAt(j++) - '0');
      }
      j += 6;
      int code = 0;
      while (row.charAt(j) != ' ') {
        code = code * 10 + (row.charAt(j++) - '0');
      }
      boolean ok = row.charAt(j + 4) == 'y';
      
      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}