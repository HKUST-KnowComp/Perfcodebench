import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int firstSpace = row.indexOf(' ');
      int secondSpace = row.indexOf(' ', firstSpace + 1);
      int thirdSpace = row.indexOf(' ', secondSpace + 1);

      int src = Integer.parseInt(row.substring(5, firstSpace));
      int lat = Integer.parseInt(row.substring(firstSpace + 5, secondSpace));
      int code = Integer.parseInt(row.substring(secondSpace + 6, thirdSpace));
      boolean ok = row.substring(thirdSpace + 4).equals("yes");

      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}