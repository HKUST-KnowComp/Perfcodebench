import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int len = row.length();
            int commaCount = 0;
            int b = 0, c = 0;
            boolean hot = false;
            int start = 0;

            for (int i = 0; i <= len; i++) {
                if (i == len || row.charAt(i) == ',') {
                    if (commaCount == 2) {
                        b = parsePositiveInt(row, start, i);
                    } else if (commaCount == 3) {
                        c = parsePositiveInt(row, start, i);
                    } else if (commaCount == 4) {
                        hot = (i - start == 3 && row.charAt(start) == 'h' && row.charAt(start + 1) == 'o' && row.charAt(start + 2) == 't');
                        break;
                    }
                    start = i + 1;
                    commaCount++;
                }
            }

            long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }

    private static int parsePositiveInt(String s, int start, int end) {
        int res = 0;
        for (int i = start; i < end; i++) {
            res = res * 10 + (s.charAt(i) - '0');
        }
        return res;
    }
}