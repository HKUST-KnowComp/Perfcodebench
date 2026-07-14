import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      String[] tokens = row.split(" ");
      int src = Integer.parseInt(tokens[0].substring(5));
      int lat = Integer.parseInt(tokens[1].split("=")[1]);
      int code = Integer.parseInt(tokens[2].split("=")[1]);
      boolean ok = tokens[3].split("=")[1].equals("yes");
      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}