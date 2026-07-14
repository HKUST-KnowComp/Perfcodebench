import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;          // FNV offset basis
    final long prime = 1099511628211L;         // FNV prime

    for (String row : rows) {
      char[] buf = row.toCharArray();
      int pos = 0;

      // skip "u="
      pos += 2;
      int u = 0;
      while (buf[pos] != '&') {
        u = u * 10 + (buf[pos++] - '0');
      }
      pos++; // skip '&'

      // skip "s="
      pos += 2;
      int s = 0;
      while (buf[pos] != '&') {
        s = s * 10 + (buf[pos++] - '0');
      }
      pos++; // skip '&'

      // skip "f="
      pos += 2;
      int f = 0;
      while (pos < buf.length) {
        f = f * 10 + (buf[pos++] - '0');
      }

      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= prime;
    }
    return hash;
  }
}