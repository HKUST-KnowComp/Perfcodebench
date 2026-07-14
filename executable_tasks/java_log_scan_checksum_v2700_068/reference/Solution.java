import java.util.*;

public class Solution {
  static int parseIntAt(String s, int start) {
    int v = 0;
    for (int i = start; i < s.length(); i++) {
      char c = s.charAt(i);
      if (c < '0' || c > '9') break;
      v = v * 10 + (c - '0');
    }
    return v;
  }

  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int srcPos = row.indexOf("src=S");
      int latPos = row.indexOf(" lat=");
      int codePos = row.indexOf(" code=");
      int okPos = row.indexOf(" ok=");
      int src = parseIntAt(row, srcPos + 5);
      int lat = parseIntAt(row, latPos + 5);
      int code = parseIntAt(row, codePos + 6);
      boolean ok = row.charAt(okPos + 4) == 'y';
      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}
