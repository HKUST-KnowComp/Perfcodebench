import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int u, s, f;
      String[] parts = row.split("&");
      String part0 = parts[0];
      String part1 = parts[1];
      String part2 = parts[2];
      u = Character.digit(part0.charAt(2), 10) * 10 + Character.digit(part0.charAt(3), 10);
      s = Character.digit(part1.charAt(6), 10) * 10 + Character.digit(part1.charAt(7), 10);
      f = Character.digit(part2.charAt(5), 10) * 10 + Character.digit(part2.charAt(6), 10);
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}