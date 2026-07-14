import java.util.List;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L; // FNV-1a 64-bit offset basis
    final long prime = 1099511628211L; // FNV-1a 64-bit prime

    for (int r = 0, n = rows.size(); r < n; r++) {
      String s = rows.get(r);
      int i = 5; // skip over {"u":

      // parse u
      int u = 0;
      char ch;
      while (true) {
        ch = s.charAt(i);
        if (ch < '0' || ch > '9') break;
        u = u * 10 + (ch - '0');
        i++;
      }

      // skip ,"s": (comma + quote + s + quote + colon)
      i += 5;

      // parse s
      int sval = 0;
      while (true) {
        ch = s.charAt(i);
        if (ch < '0' || ch > '9') break;
        sval = sval * 10 + (ch - '0');
        i++;
      }

      // skip ,"f": (comma + quote + f + quote + colon)
      i += 5;

      // parse boolean
      boolean f;
      ch = s.charAt(i);
      if (ch == 't') { // true
        f = true;
        i += 4;
      } else { // false
        f = false;
        i += 5;
      }

      long v = (long) ((u & 1023) + sval + (f ? 17 : 0));
      hash ^= (v + 1);
      hash *= prime;
    }
    return hash;
  }
}