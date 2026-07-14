import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int firstComma = row.indexOf(',', 5);
      int secondComma = row.indexOf(',', firstComma + 1);
      int u = 0;
      for (int i = 5; i < firstComma; i++) {
        u = u * 10 + (row.charAt(i) - '0');
      }
      int s = 0;
      for (int i = firstComma + 5; i < secondComma; i++) {
        s = s * 10 + (row.charAt(i) - '0');
      }
      boolean f = row.charAt(secondComma + 5) == 't';
      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}