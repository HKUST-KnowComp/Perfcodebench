import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int src = 0, lat = 0, code = 0;
      boolean ok = false;
      String[] parts = row.split(" ");
      for (String part : parts) {
        if (part.startsWith("src=S")) {
          src = Integer.parseInt(part.substring(5));
        } else if (part.startsWith("lat=")) {
          lat = Integer.parseInt(part.substring(4));
        } else if (part.startsWith("code=")) {
          code = Integer.parseInt(part.substring(5));
        } else if (part.startsWith("ok=")) {
          ok = part.substring(3).equals("yes");
        }
      }
      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}