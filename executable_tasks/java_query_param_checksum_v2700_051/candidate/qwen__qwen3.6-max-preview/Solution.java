import java.util.List;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      int u = 0, s = 0, f = 0;
      int i = 0;
      char c;
      int sign;

      // Parse u
      while (i < len) { c = row.charAt(i); if (c == '-' || (c >= '0' && c <= '9')) break; i++; }
      sign = 1;
      if (i < len && row.charAt(i) == '-') { sign = -1; i++; }
      while (i < len) { c = row.charAt(i); if (c < '0' || c > '9') break; u = u * 10 + (c - '0'); i++; }
      u *= sign;

      // Parse s
      while (i < len) { c = row.charAt(i); if (c == '-' || (c >= '0' && c <= '9')) break; i++; }
      sign = 1;
      if (i < len && row.charAt(i) == '-') { sign = -1; i++; }
      while (i < len) { c = row.charAt(i); if (c < '0' || c > '9') break; s = s * 10 + (c - '0'); i++; }
      s *= sign;

      // Parse f
      while (i < len) { c = row.charAt(i); if (c == '-' || (c >= '0' && c <= '9')) break; i++; }
      sign = 1;
      if (i < len && row.charAt(i) == '-') { sign = -1; i++; }
      while (i < len) { c = row.charAt(i); if (c < '0' || c > '9') break; f = f * 10 + (c - '0'); i++; }
      f *= sign;

      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}