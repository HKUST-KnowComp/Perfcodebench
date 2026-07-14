import java.util.List;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int comma1 = row.indexOf(',', 5);
      int u = 0;
      for (int j = 5; j < comma1; ++j) {
        u = u * 10 + (row.charAt(j) - '0');
      }
      int i = comma1 + 5;
      int comma2 = row.indexOf(',', i);
      int s = 0;
      for (int j = i; j < comma2; ++j) {
        s = s * 10 + (row.charAt(j) - '0');
      }
      i = comma2 + 5;
      boolean f = row.charAt(i) == 't';
      int sum = (u & 1023) + s + (f ? 17 : 0);
      long v = sum;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}