import java.util.List;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (int i = 0; i < rows.size(); i++) {
      String row = rows.get(i);
      int pos = 5; // after "src=S"
      // parse src
      int src = 0;
      while (pos < row.length() && row.charAt(pos) >= '0' && row.charAt(pos) <= '9') {
        src = src * 10 + (row.charAt(pos) - '0');
        pos++;
      }
      pos++; // skip space
      pos += 4; // skip "lat="
      // parse lat
      int lat = 0;
      while (pos < row.length() && row.charAt(pos) >= '0' && row.charAt(pos) <= '9') {
        lat = lat * 10 + (row.charAt(pos) - '0');
        pos++;
      }
      pos++; // skip space
      pos += 5; // skip "code="
      // parse code
      int code = 0;
      while (pos < row.length() && row.charAt(pos) >= '0' && row.charAt(pos) <= '9') {
        code = code * 10 + (row.charAt(pos) - '0');
        pos++;
      }
      pos++; // skip space
      pos += 3; // skip "ok="
      // ok is 'y' for yes, else no
      boolean ok = (row.charAt(pos) == 'y');
      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}