import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int pos1 = row.indexOf('&');
      int pos2 = row.indexOf('&', pos1 + 1);
      int u = 0;
      for (int i = 2; i < pos1; i++) {
        u = u * 10 + (row.charAt(i) - '0');
      }
      int s = 0;
      for (int i = pos1 + 7; i < pos2; i++) {
        s = s * 10 + (row.charAt(i) - '0');
      }
      int f = 0;
      for (int i = pos2 + 6; i < row.length(); i++) {
        f = f * 10 + (row.charAt(i) - '0');
      }
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}