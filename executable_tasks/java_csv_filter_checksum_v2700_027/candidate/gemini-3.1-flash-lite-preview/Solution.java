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

            int b = parseFast(row, comma2 + 1, comma3);
            int c = parseFast(row, comma3 + 1, comma4);
            
            boolean hot = false;
            int startHot = comma4 + 1;
            int endHot = row.indexOf(',', startHot);
            if (endHot == -1) endHot = len;
            
            if (endHot - startHot == 3 && row.charAt(startHot) == 'h' && row.charAt(startHot + 1) == 'o' && row.charAt(startHot + 2) == 't') {
                hot = true;
            }

            long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }

    private static int parseFast(String s, int start, int end) {
        int res = 0;
        for (int i = start; i < end; i++) {
            res = res * 10 + (s.charAt(i) - '0');
        }
        return res;
    }
}