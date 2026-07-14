import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      String[] parts = row.split(" ");
      int src = Integer.parseInt(parts[0].substring(5));
      int lat = Integer.parseInt(parts[1].substring(4));
      int code = Integer.parseInt(parts[2].substring(5));
      boolean ok = parts[3].substring(3).equals("yes");
      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}