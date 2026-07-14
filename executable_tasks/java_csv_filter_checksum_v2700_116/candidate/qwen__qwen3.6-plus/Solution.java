import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    int size = rows.size();
    for (int i = 0; i < size; i++) {
      String row = rows.get(i);
      int idx1 = row.indexOf(',');
      int idx2 = row.indexOf(',', idx1 + 1);
      int idx3 = row.indexOf(',', idx2 + 1);
      int idx4 = row.indexOf(',', idx3 + 1);
      
      int b = 0;
      int k = idx2 + 1;
      boolean negB = false;
      if (k < idx3 && row.charAt(k) == '-') { negB = true; k++; }
      for (; k < idx3; k++) b = b * 10 + (row.charAt(k) - '0');
      if (negB) b = -b;
      
      int c = 0;
      k = idx3 + 1;
      boolean negC = false;
      if (k < idx4 && row.charAt(k) == '-') { negC = true; k++; }
      for (; k < idx4; k++) c = c * 10 + (row.charAt(k) - '0');
      if (negC) c = -c;
      
      int hotStart = idx4 + 1;
      int hotEnd = row.indexOf(',', hotStart);
      if (hotEnd == -1) hotEnd = row.length();
      boolean hot = (hotEnd - hotStart == 3) && 
                    row.charAt(hotStart) == 'h' && 
                    row.charAt(hotStart+1) == 'o' && 
                    row.charAt(hotStart+2) == 't';
                    
      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}