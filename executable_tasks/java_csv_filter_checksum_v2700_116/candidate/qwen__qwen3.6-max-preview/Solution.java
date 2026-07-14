import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int c1 = row.indexOf(',');
            int c2 = row.indexOf(',', c1 + 1);
            int c3 = row.indexOf(',', c2 + 1);
            int c4 = row.indexOf(',', c3 + 1);
            
            int b = 0;
            int idx = c2 + 1;
            boolean bNeg = false;
            if (idx < c3) {
                char ch = row.charAt(idx);
                if (ch == '-') { bNeg = true; idx++; }
                else if (ch == '+') { idx++; }
            }
            while (idx < c3) {
                b = b * 10 + (row.charAt(idx) - '0');
                idx++;
            }
            if (bNeg) b = -b;
            
            int c = 0;
            idx = c3 + 1;
            boolean cNeg = false;
            if (idx < c4) {
                char ch = row.charAt(idx);
                if (ch == '-') { cNeg = true; idx++; }
                else if (ch == '+') { idx++; }
            }
            while (idx < c4) {
                c = c * 10 + (row.charAt(idx) - '0');
                idx++;
            }
            if (cNeg) c = -c;
            
            boolean hot = c4 + 3 < row.length() && 
                          row.charAt(c4 + 1) == 'h' && 
                          row.charAt(c4 + 2) == 'o' && 
                          row.charAt(c4 + 3) == 't' && 
                          (c4 + 4 == row.length() || row.charAt(c4 + 4) == ',');
            
            long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}