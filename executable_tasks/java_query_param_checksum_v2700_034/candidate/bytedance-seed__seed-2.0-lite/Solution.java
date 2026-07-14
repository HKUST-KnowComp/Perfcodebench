import java.util.List;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int firstAmp = row.indexOf('&');
      int secondAmp = row.indexOf('&', firstAmp + 1);
      int u = Integer.parseInt(row, 2, firstAmp, 10);
      int s = Integer.parseInt(row, firstAmp + 7, secondAmp, 10);
      int f = Integer.parseInt(row, secondAmp + 6, row.length(), 10);
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}