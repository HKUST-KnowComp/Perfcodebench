import java.util.List;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int p1 = row.indexOf(',');
            int p2 = row.indexOf(',', p1 + 1);
            int p3 = row.indexOf(',', p2 + 1);
            int p4 = row.indexOf(',', p3 + 1);
            int p5 = row.indexOf(',', p4 + 1);
            if (p5 == -1) p5 = row.length();
            
            int b = fastParseInt(row, p2 + 1, p3);
            int c = fastParseInt(row, p3 + 1, p4);
            boolean hot = (p5 - p4 == 4) && row.charAt(p4 + 1) == 'h' && row.charAt(p4 + 2) == 'o' && row.charAt(p4 + 3) == 't';
            
            long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }

    private static int fastParseInt(String s, int from, int to) {
        int val = 0;
        int i = from;
        if (i < to && s.charAt(i) == '-') {
            i++;
            while (i < to) {
                val = val * 10 + (s.charAt(i) - '0');
                i++;
            }
            return -val;
        }
        while (i < to) {
            val = val * 10 + (s.charAt(i) - '0');
            i++;
        }
        return val;
    }
}