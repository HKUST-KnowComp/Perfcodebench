import java.util.List;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      String[] parts = row.split(",");
      int b = Integer.parseInt(parts[2]);
      int c = Integer.parseInt(parts[3]);
      boolean hot = parts[4].equals("hot");
      long v = (b > 300 ? (long)b + c : 0) + (hot ? 29 : 0);
      hash ^= v + 1;
      hash *= 1099511628211L;
    }
    return hash;
  }
}