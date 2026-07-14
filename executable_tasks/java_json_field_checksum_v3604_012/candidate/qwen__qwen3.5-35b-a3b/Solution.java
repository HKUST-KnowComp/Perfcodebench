import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int u_start = 5;
      int u_end = row.indexOf(',', u_start);
      int u = 0;
      for (int i = u_start; i < u_end; i++) {
        u = u * 10 + (row.charAt(i) - '0');
      }

      int s_start = row.indexOf("\"s\":", u_end + 1);
      int s_colon = s_start + 3;
      int s_value_start = s_colon + 1;
      int s_end = row.indexOf(',', s_value_start);
      int s = 0;
      for (int i = s_value_start; i < s_end; i++) {
        s = s * 10 + (row.charAt(i) - '0');
      }

      int f_start = row.indexOf("\"f\":", s_end + 1);
      int f_colon = f_start + 3;
      int f_value_start = f_colon + 1;
      boolean f = row.charAt(f_value_start) == 't';

      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}