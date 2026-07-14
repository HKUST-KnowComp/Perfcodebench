import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int p1 = row.indexOf(',', 0);
            int p2 = row.indexOf(',', p1 + 1);
            int p3 = row.indexOf(',', p2 + 1);
            int p4 = row.indexOf(',', p3 + 1);
            int p5 = row.indexOf(',', p4 + 1);
            if (p5 == -1) p5 = row.length();
            
            // parse field 2 (b)
            int b = parsePositiveInt(row, p2 + 1, p3);
            // parse field 3 (c)
            int c = parsePositiveInt(row, p3 + 1, p4);
            // check if field 4 is "hot"
            boolean hot = (p5 - p4 - 1 == 3)
                    && row.charAt(p4 + 1) == 'h'
                    && row.charAt(p4 + 2) == 'o'
                    && row.charAt(p4 + 3) == 't';
            
            long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }

    private static int parsePositiveInt(String s, int start, int end) {
        int result = 0;
        for (int i = start; i < end; i++) {
            result = result * 10 + (s.charAt(i) - '0');
        }
        return result;
    }
}