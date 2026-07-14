import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int comma1 = row.indexOf(',', 5);
      int u = Integer.parseInt(row.substring(5, comma1));
      int s_colon = comma1 + 4;
      int comma2 = row.indexOf(',', s_colon + 1);
      int s = Integer.parseInt(row.substring(s_colon + 1, comma2));
      int f_colon = comma2 + 4;
      int comma3 = row.indexOf(',', f_colon + 1);
      String f_str = row.substring(f_colon + 1, comma3);
      boolean f = f_str.equals("true");
      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}