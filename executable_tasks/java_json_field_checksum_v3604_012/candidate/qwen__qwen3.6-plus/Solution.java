import java.util.List;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (int i = 0; i < rows.size(); i++) {
      String row = rows.get(i);
      int p = row.indexOf(':') + 1;
      int u = 0;
      while (true) {
        char c = row.charAt(p);
        if (c < '0' || c > '9') break;
        u = u * 10 + (c - '0');
        p++;
      }
      p = row.indexOf(':', p) + 1;
      int s = 0;
      while (true) {
        char c = row.charAt(p);
        if (c < '0' || c > '9') break;
        s = s * 10 + (c - '0');
        p++;
      }
      p = row.indexOf(':', p) + 1;
      boolean f = row.charAt(p) == 't';
      
      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}