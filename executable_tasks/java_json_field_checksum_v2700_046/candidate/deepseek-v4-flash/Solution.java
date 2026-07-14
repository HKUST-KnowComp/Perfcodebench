import java.util.List;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      int pos = 5; // after {"u":
      // parse u
      int u = 0;
      while (pos < len && row.charAt(pos) != ',') {
        u = u * 10 + (row.charAt(pos) - '0');
        pos++;
      }
      pos++; // skip ','
      // skip to colon for "s"
      while (row.charAt(pos) != ':') pos++;
      pos++; // skip ':'
      int s = 0;
      while (pos < len && row.charAt(pos) != ',') {
        s = s * 10 + (row.charAt(pos) - '0');
        pos++;
      }
      pos++; // skip ','
      // skip to colon for "f"
      while (row.charAt(pos) != ':') pos++;
      pos++; // skip ':'
      boolean f = (row.charAt(pos) == 't'); // 't' or 'f'
      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}