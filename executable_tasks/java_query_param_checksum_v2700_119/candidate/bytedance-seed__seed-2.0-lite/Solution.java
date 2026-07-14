import java.util.List;

public class Solution {
  private static final long FNV_OFFSET = 1469598103934665603L;
  private static final long FNV_PRIME = 1099511628211L;
  private static final int MASK_2047 = 2047;
  private static final long F_MULTIPLIER = 19L;

  public static long run(List<String> rows) {
    long hash = FNV_OFFSET;
    for (String row : rows) {
      String[] parts = row.split("&", 4);
      int u = Integer.parseInt(parts[0].substring(2));
      int s = Integer.parseInt(parts[1].substring(6));
      int f = Integer.parseInt(parts[2].substring(5));
      long v = (u & MASK_2047) + s + f * F_MULTIPLIER;
      hash ^= (v + 1);
      hash *= FNV_PRIME;
    }
    return hash;
  }
}