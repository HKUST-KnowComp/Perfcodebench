public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int u = getInt(row, 2);
      int s = getInt(row, 6);
      int f = getInt(row, 5);
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
  private static int getInt(String row, int start) {
    int endIndex = row.indexOf('&', start);
    if (endIndex == -1) endIndex = row.length();
    return Integer.parseInt(row.substring(start, endIndex));
  }
}