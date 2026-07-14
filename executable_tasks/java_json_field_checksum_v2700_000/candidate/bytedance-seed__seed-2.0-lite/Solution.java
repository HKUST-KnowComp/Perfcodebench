import java.util.List;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      // Parse u field value from fixed position after {"u":
      int firstComma = row.indexOf(',', 5);
      int u = 0;
      for (int i = 5; i < firstComma; i++) {
        u = u * 10 + (row.charAt(i) - '0');
      }
      // Parse s field value after the ","s":" sequence
      int sStart = firstComma + 5;
      int secondComma = row.indexOf(',', sStart);
      int s = 0;
      for (int i = sStart; i < secondComma; i++) {
        s = s * 10 + (row.charAt(i) - '0');
      }
      // Parse f flag by checking the first character of its value
      boolean f = row.charAt(secondComma + 5) == 't';
      // Exact same hash calculation as original to preserve correctness
      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}