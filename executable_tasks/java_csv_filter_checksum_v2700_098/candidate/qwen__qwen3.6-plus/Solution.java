import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      int commaCount = 0;
      int fieldStart = 0;
      int b = 0, c = 0;
      boolean hot = false;
      
      for (int i = 0; i < len; i++) {
        if (row.charAt(i) == ',') {
          if (commaCount == 2) {
            int res = 0;
            int j = fieldStart;
            boolean neg = false;
            if (j < i && row.charAt(j) == '-') { neg = true; j++; }
            for (; j < i; j++) res = res * 10 + (row.charAt(j) - '0');
            b = neg ? -res : res;
          } else if (commaCount == 3) {
            int res = 0;
            int j = fieldStart;
            boolean neg = false;
            if (j < i && row.charAt(j) == '-') { neg = true; j++; }
            for (; j < i; j++) res = res * 10 + (row.charAt(j) - '0');
            c = neg ? -res : res;
          } else if (commaCount == 4) {
            int fLen = i - fieldStart;
            hot = fLen == 3 && row.charAt(fieldStart) == 'h' && 
                  row.charAt(fieldStart + 1) == 'o' && row.charAt(fieldStart + 2) == 't';
            break;
          }
          fieldStart = i + 1;
          commaCount++;
        }
      }
      
      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}