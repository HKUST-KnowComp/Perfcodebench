import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      String[] parts = row.split(",");
      int b, c;
      try {
        b = Integer.parseInt(parts[2]);
        c = Integer.parseInt(parts[3]);
      } catch (NumberFormatException e) {
        // Handle or skip invalid row
        continue;
      }
      boolean hot = parts[4].equals("hot");
      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}