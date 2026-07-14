import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int len = row.length();
            int comma1 = row.indexOf(',');
            int comma2 = row.indexOf(',', comma1 + 1);
            int comma3 = row.indexOf(',', comma2 + 1);
            int comma4 = row.indexOf(',', comma3 + 1);

            int b = 0;
            for (int i = comma2 + 1; i < comma3; i++) {
                b = b * 10 + (row.charAt(i) - '0');
            }

            int c = 0;
            for (int i = comma3 + 1; i < comma4; i++) {
                c = c * 10 + (row.charAt(i) - '0');
            }

            boolean hot = false;
            if (len - comma4 == 4 && row.charAt(comma4 + 1) == 'h' && row.charAt(comma4 + 2) == 'o' && row.charAt(comma4 + 3) == 't') {
                hot = true;
            }

            long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}