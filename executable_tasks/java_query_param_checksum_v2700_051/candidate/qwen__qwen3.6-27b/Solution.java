import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    int n = rows.size();
    for (int i = 0; i < n; i++) {
      String row = rows.get(i);
      int len = row.length();

      int eq1 = row.indexOf('=');
      int amp1 = row.indexOf('&', eq1);
      int u = 0;
      for (int j = eq1 + 1; j < amp1; j++) {
        u = u * 10 + (row.charAt(j) - '0');
      }

      int eq2 = row.indexOf('=', amp1);
      int amp2 = row.indexOf('&', eq2);
      int s = 0;
      for (int j = eq2 + 1; j < amp2; j++) {
        s = s * 10 + (row.charAt(j) - '0');
      }

      int eq3 = row.indexOf('=', amp2);
      int f = 0;
      for (int j = eq3 + 1; j < len; j++) {
        f = f * 10 + (row.charAt(j) - '0');
      }

      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}