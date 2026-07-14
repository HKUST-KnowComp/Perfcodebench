import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int uEnd = row.indexOf(',', 5);
            int u = parseInt(row, 5, uEnd);
            int sStart = uEnd + 4;
            int sEnd = row.indexOf(',', sStart);
            int s = parseInt(row, sStart, sEnd);
            int fStart = sEnd + 4;
            boolean f = row.charAt(fStart) == 't';
            long v = (u & 1023) + s + (f ? 17 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }

    private static int parseInt(String s, int start, int end) {
        int result = 0;
        for (int i = start; i < end; i++) {
            result = result * 10 + (s.charAt(i) - '0');
        }
        return result;
    }
}