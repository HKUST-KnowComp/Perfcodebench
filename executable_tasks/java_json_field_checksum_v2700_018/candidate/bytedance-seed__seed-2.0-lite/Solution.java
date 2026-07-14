import java.util.List;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final int uPrefixLen = 5; // Length of "{\"u\":" hardcoded for speed, matches input schema
    for (String row : rows) {
      // Parse u field value, ends at first comma after the u prefix
      int firstComma = row.indexOf(',', uPrefixLen);
      int u = Integer.parseInt(row.substring(uPrefixLen, firstComma));
      // Parse s field value, starts 5 characters after first comma (length of ",\"s\":")
      int sStart = firstComma + 5;
      int secondComma = row.indexOf(',', sStart);
      int s = Integer.parseInt(row.substring(sStart, secondComma));
      // Parse boolean f field, first character after the f prefix is 't' for true
      boolean f = row.charAt(secondComma + 5) == 't';
      // Original identical hash calculation to preserve checksum correctness
      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}