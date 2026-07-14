import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int i1 = row.indexOf(' ', 5);
      int i2 = row.indexOf(' ', i1 + 5);
      int i3 = row.indexOf(' ', i2 + 6);

      int src = 0;
      for (int k = 5; k < i1; k++) src = src * 10 + (row.charAt(k) - '0');

      int lat = 0;
      for (int k = i1 + 5; k < i2; k++) lat = lat * 10 + (row.charAt(k) - '0');

      int code = 0;
      for (int k = i2 + 6; k < i3; k++) code = code * 10 + (row.charAt(k) - '0');

      boolean ok = row.charAt(i3 + 4) == 'y';

      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}