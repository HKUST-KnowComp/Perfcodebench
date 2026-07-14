import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int commaCount = 0;
      int start = 0;
      int b = 0, c = 0;
      boolean hot = false;
      for (int i = 0; i < row.length(); i++) {
        char ch = row.charAt(i);
        if (ch == ',') {
          commaCount++;
          if (commaCount == 2) {
            b = Integer.parseInt(row.substring(start, i));
            start = i + 1;
          } else if (commaCount == 3) {
            c = Integer.parseInt(row.substring(start, i));
            start = i + 1;
          }
        } else if (commaCount == 4 && i == row.length() - 3 && row.substring(start).equals("hot")) {
          hot = true;
        }
      }
      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}