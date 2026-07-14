import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int p0 = row.indexOf(',');
            int p1 = row.indexOf(',', p0 + 1);
            int p2 = row.indexOf(',', p1 + 1);
            int p3 = row.indexOf(',', p2 + 1);
            
            int b = parseInt(row, p1 + 1, p2);
            int c = parseInt(row, p2 + 1, p3);
            boolean hot = row.startsWith("hot", p3 + 1) && (p3 + 4 == row.length() || row.charAt(p3 + 4) == ',');
            
            long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }

    private static int parseInt(String s, int begin, int end) {
        int res = 0;
        int i = begin;
        if (i < end && s.charAt(i) == '-') {
            i++;
            while (i < end) {
                res = res * 10 - (s.charAt(i) - '0');
                i++;
            }
            return res;
        }
        while (i < end) {
            res = res * 10 + (s.charAt(i) - '0');
            i++;
        }
        return res;
    }
}