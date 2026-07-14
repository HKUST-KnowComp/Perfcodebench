public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int u = getInt(row, 2, 3);
      int s = getInt(row, 6, 4);
      int f = getInt(row, 5, 3);
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
  private static int getInt(String str, int start, int len) {
    int end = start + len;
    int result = 0;
    for (int i = start; i < end; i++) {
      char c = str.charAt(i);
      result = result * 10 + (c - '0');
    }
    return result;
  }
}