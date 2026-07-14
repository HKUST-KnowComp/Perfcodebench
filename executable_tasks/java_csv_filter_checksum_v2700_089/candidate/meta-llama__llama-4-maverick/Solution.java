import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int start = 0, end = 0;
      int commaCount = 0;
      for (int i = 0; i < row.length(); i++) {
        if (row.charAt(i) == ',') {
          commaCount++;
          if (commaCount == 2) {
            start = i + 1;
          } else if (commaCount == 3) {
            end = i;
            break;
          }
        }
      }
      int b = Integer.parseInt(row.substring(start, end));
      start = end + 1;
      end = row.indexOf(',', start);
      int c = Integer.parseInt(row.substring(start, end));
      boolean hot = row.charAt(end + 1) == 'h' && row.charAt(end + 2) == 'o' && row.charAt(end + 3) == 't';
      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}