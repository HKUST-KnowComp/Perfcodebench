import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int u_end = row.indexOf(',', 5);
      int u = Integer.parseInt(row.substring(5, u_end));
      
      int s_start = u_end + 5;
      int s_end = row.indexOf(',', s_start);
      int s = Integer.parseInt(row.substring(s_start, s_end));
      
      int f_start = s_end + 5;
      boolean f = row.charAt(f_start + 4) == ',';
      
      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}