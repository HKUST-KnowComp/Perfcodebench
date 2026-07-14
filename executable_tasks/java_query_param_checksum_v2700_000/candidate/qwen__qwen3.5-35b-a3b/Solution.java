import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int pos1 = row.indexOf('&');
      int pos2 = row.indexOf('&', pos1 + 1);
      int u = Integer.parseInt(row.substring(2, pos1));
      int s = Integer.parseInt(row.substring(pos1 + 7, pos2));
      int f = Integer.parseInt(row.substring(pos2 + 6, row.length()));
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}