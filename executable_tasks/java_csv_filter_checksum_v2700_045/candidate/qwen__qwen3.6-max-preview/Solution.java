import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int p0 = row.indexOf(',');
            int p1 = row.indexOf(',', p0 + 1);
            int p2 = row.indexOf(',', p1 + 1);
            int p3 = row.indexOf(',', p2 + 1);
            
            int b = 0;
            int bStart = p1 + 1;
            boolean bNeg = row.charAt(bStart) == '-';
            for (int j = bNeg ? bStart + 1 : bStart; j < p2; j++) {
                b = b * 10 + (row.charAt(j) - '0');
            }
            if (bNeg) b = -b;
            
            int c = 0;
            int cStart = p2 + 1;
            boolean cNeg = row.charAt(cStart) == '-';
            for (int j = cNeg ? cStart + 1 : cStart; j < p3; j++) {
                c = c * 10 + (row.charAt(j) - '0');
            }
            if (cNeg) c = -c;
            
            int hStart = p3 + 1;
            boolean hot = row.charAt(hStart) == 'h' && row.charAt(hStart + 1) == 'o' && row.charAt(hStart + 2) == 't' && (hStart + 3 >= row.length() || row.charAt(hStart + 3) == ',');
            
            long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}