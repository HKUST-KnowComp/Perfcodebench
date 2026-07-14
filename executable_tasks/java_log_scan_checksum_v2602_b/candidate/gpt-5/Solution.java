import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    int n = rows.size();
    for (int i = 0; i < n; i++) {
      String row = rows.get(i);
      int len = row.length();
      int p = 5; // after "src=S"

      int src = 0;
      while (p < len) {
        char c = row.charAt(p);
        if (c < '0' || c > '9') break;
        src = src * 10 + (c - '0');
        p++;
      }

      p += 5; // skip " lat="
      int lat = 0;
      while (p < len) {
        char c = row.charAt(p);
        if (c < '0' || c > '9') break;
        lat = lat * 10 + (c - '0');
        p++;
      }

      p += 6; // skip " code="
      int code = 0;
      while (p < len) {
        char c = row.charAt(p);
        if (c < '0' || c > '9') break;
        code = code * 10 + (c - '0');
        p++;
      }

      p += 4; // skip " ok="
      boolean ok = row.charAt(p) == 'y';

      int vInt = (src & 255) + lat + code + (ok ? 31 : 0);
      long v = (long) vInt;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}