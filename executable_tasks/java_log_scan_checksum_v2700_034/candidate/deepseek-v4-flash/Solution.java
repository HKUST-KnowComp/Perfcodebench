import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int idx = row.indexOf(' ');
      int srcStart = 5; // after "src=S"
      int srcEnd = idx;
      int src = parseInt(row, srcStart, srcEnd);
      int latStart = idx + 5; // after " lat="
      int latEnd = row.indexOf(' ', latStart);
      int lat = parseInt(row, latStart, latEnd);
      int codeStart = latEnd + 6; // after " code="
      int codeEnd = row.indexOf(' ', codeStart);
      int code = parseInt(row, codeStart, codeEnd);
      int okStart = codeEnd + 4; // after " ok="
      boolean ok = (row.charAt(okStart) == 'y');
      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }

  private static int parseInt(String s, int start, int end) {
    int result = 0;
    for (int i = start; i < end; i++) {
      result = result * 10 + (s.charAt(i) - '0');
    }
    return result;
  }
}