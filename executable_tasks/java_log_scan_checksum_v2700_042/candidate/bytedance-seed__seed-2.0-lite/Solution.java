import java.util.List;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      // Parse src value after leading "src=S"
      int src = 0;
      int i = 5;
      while (i < len) {
        char c = row.charAt(i);
        if (c == ' ') break;
        src = src * 10 + (c - '0');
        i++;
      }
      // Advance to first digit of lat, past " lat="
      i += 5;
      int lat = 0;
      while (i < len) {
        char c = row.charAt(i);
        if (c == ' ') break;
        lat = lat * 10 + (c - '0');
        i++;
      }
      // Advance to first digit of code, past " code="
      i += 6;
      int code = 0;
      while (i < len) {
        char c = row.charAt(i);
        if (c == ' ') break;
        code = code * 10 + (c - '0');
        i++;
      }
      // Check ok state: last char is 's' for "yes", 'o' for "no"
      boolean ok = row.charAt(len - 1) == 's';
      // Exact original hash calculation preserved for correctness
      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}