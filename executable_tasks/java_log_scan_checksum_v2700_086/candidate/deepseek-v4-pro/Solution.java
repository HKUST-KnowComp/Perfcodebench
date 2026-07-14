import java.util.List;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      int pos = 5; // skip "src=S"
      int src = 0;
      char c;
      while ((c = row.charAt(pos)) != ' ') {
        src = src * 10 + (c - '0');
        pos++;
      }
      pos++; // skip space
      pos += 4; // skip "lat="
      int lat = 0;
      while ((c = row.charAt(pos)) != ' ') {
        lat = lat * 10 + (c - '0');
        pos++;
      }
      pos++; // skip space
      pos += 5; // skip "code="
      int code = 0;
      while ((c = row.charAt(pos)) != ' ') {
        code = code * 10 + (c - '0');
        pos++;
      }
      pos++; // skip space
      pos += 3; // skip "ok="
      boolean ok = row.charAt(pos) == 'y';
      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}