import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int latStart = row.indexOf(" lat=");
      int codeStart = row.indexOf(" code=");
      int okStart = row.indexOf(" ok=");
      
      int src = parseDigits(row, 5, latStart);
      int lat = parseDigits(row, latStart + 5, codeStart);
      int code = parseDigits(row, codeStart + 6, okStart);
      boolean ok = row.charAt(okStart + 4) == 'y';
      
      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
  
  private static int parseDigits(String s, int start, int end) {
    int num = 0;
    for (int i = start; i < end; i++) {
      num = num * 10 + (s.charAt(i) - '0');
    }
    return num;
  }
}