import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (int i = 0; i < rows.size(); i++) {
      String row = rows.get(i);
      int len = row.length();
      int idx = 6; // after "src=S"
      int end = idx;
      while (end < len && row.charAt(end) != ' ') end++;
      int src = parseInt(row, idx, end);
      // skip " lat=" (5 char)
      idx = end + 5;
      end = idx;
      while (end < len && row.charAt(end) != ' ') end++;
      int lat = parseInt(row, idx, end);
      // skip " code=" (6 char)
      idx = end + 6;
      end = idx;
      while (end < len && row.charAt(end) != ' ') end++;
      int code = parseInt(row, idx, end);
      // skip " ok=" (4 char)
      idx = end + 4;
      boolean ok = row.charAt(idx) == 'y';
      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }

  private static int parseInt(String s, int start, int end) {
    int num = 0;
    for (int i = start; i < end; i++) {
      num = num * 10 + (s.charAt(i) - '0');
    }
    return num;
  }
}