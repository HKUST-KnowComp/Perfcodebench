import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    int size = rows.size();
    for (int i = 0; i < size; i++) {
      String row = rows.get(i);
      int len = row.length();
      int i1 = row.indexOf('&');
      int i2 = row.indexOf('&', i1 + 1);

      int u = 0;
      int p = 2;
      if (p < i1 && row.charAt(p) == '-') p++;
      for (; p < i1; p++) u = u * 10 + (row.charAt(p) - '0');

      int s = 0;
      p = i1 + 7;
      if (p < i2 && row.charAt(p) == '-') p++;
      for (; p < i2; p++) s = s * 10 + (row.charAt(p) - '0');

      int f = 0;
      p = i2 + 6;
      if (p < len && row.charAt(p) == '-') p++;
      for (; p < len; p++) f = f * 10 + (row.charAt(p) - '0');

      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}