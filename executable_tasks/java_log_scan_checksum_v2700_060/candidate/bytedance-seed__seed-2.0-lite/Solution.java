import java.util.List;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int space1 = row.indexOf(' ');
            int space2 = row.indexOf(' ', space1 + 1);
            int space3 = row.indexOf(' ', space2 + 1);
            
            int src = 0;
            for (int i = 5; i < space1; i++) {
                src = src * 10 + (row.charAt(i) - '0');
            }
            
            int lat = 0;
            for (int i = space1 + 5; i < space2; i++) {
                lat = lat * 10 + (row.charAt(i) - '0');
            }
            
            int code = 0;
            for (int i = space2 + 6; i < space3; i++) {
                code = code * 10 + (row.charAt(i) - '0');
            }
            
            boolean ok = row.charAt(space3 + 4) == 'y';
            
            long v = (src & 255) + lat + code + (ok ? 31 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}