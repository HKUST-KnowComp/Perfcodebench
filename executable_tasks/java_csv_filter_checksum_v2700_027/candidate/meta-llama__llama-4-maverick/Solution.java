import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int start = 0, end = 0, field = 0;
      int b = 0, c = 0;
      boolean hot = false;
      while (end < row.length()) {
        if (row.charAt(end) == ',') {
          String part = row.substring(start, end);
          switch (field) {
            case 2:
              b = Integer.parseInt(part);
              break;
            case 3:
              c = Integer.parseInt(part);
              break;
            case 4:
              hot = part.equals("hot");
              break;
          }
          field++;
          start = end + 1;
        }
        end++;
      }
      // Handle the last field
      if (start < row.length()) {
        String part = row.substring(start);
        switch (field) {
          case 2:
            b = Integer.parseInt(part);
            break;
          case 3:
            c = Integer.parseInt(part);
            break;
          case 4:
            hot = part.equals("hot");
            break;
        }
      }
      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}