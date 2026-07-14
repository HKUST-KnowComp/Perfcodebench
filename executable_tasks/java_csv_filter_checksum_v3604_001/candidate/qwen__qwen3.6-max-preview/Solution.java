import java.util.List;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int p0 = row.indexOf(',');
      int p1 = row.indexOf(',', p0 + 1);
      int p2 = row.indexOf(',', p1 + 1);
      int p3 = row.indexOf(',', p2 + 1);
      
      int startB = p1 + 1;
      int b = 0;
      char firstB = row.charAt(startB);
      boolean negB = firstB == '-';
      if (negB || firstB == '+') startB++;
      for (int i = startB; i < p2; i++) {
        b = b * 10 + (row.charAt(i) - '0');
      }
      if (negB) b = -b;
      
      int startC = p2 + 1;
      int c = 0;
      char firstC = row.charAt(startC);
      boolean negC = firstC == '-';
      if (negC || firstC == '+') startC++;
      for (int i = startC; i < p3; i++) {
        c = c * 10 + (row.charAt(i) - '0');
      }
      if (negC) c = -c;
      
      boolean hot = row.charAt(p3 + 1) == 'h';
      
      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}