import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      byte[] b = row.getBytes();
      int u = 0, s = 0, f = 0;
      int i = 2;                       // skip "u="
      while (b[i] != '&') { u = u * 10 + (b[i++] - '0'); }
      i += 7;                          // skip "&snum="
      while (b[i] != '&') { s = s * 10 + (b[i++] - '0'); }
      i += 6;                          // skip "&flag="
      while (i < b.length) { f = f * 10 + (b[i++] - '0'); }
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}