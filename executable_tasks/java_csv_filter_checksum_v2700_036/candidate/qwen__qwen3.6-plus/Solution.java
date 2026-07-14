import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        int size = rows.size();
        for (int i = 0; i < size; i++) {
            String row = rows.get(i);
            int len = row.length();
            int commaCount = 0;
            int start = 0;
            int b = 0, c = 0;
            boolean hot = false;
            
            for (int j = 0; j < len; j++) {
                if (row.charAt(j) == ',') {
                    commaCount++;
                    if (commaCount == 3) {
                        b = parseFastInt(row, start, j);
                        start = j + 1;
                    } else if (commaCount == 4) {
                        c = parseFastInt(row, start, j);
                        start = j + 1;
                    } else if (commaCount == 5) {
                        hot = (j - start == 3) && row.charAt(start) == 'h' && row.charAt(start + 1) == 'o' && row.charAt(start + 2) == 't';
                        break;
                    }
                }
            }
            if (commaCount == 4) {
                hot = (len - start == 3) && row.charAt(start) == 'h' && row.charAt(start + 1) == 'o' && row.charAt(start + 2) == 't';
            }
            
            long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }

    private static int parseFastInt(String s, int start, int end) {
        int res = 0;
        int i = start;
        if (i < end && s.charAt(i) == '-') {
            i++;
            for (; i < end; i++) {
                res = res * 10 + (s.charAt(i) - '0');
            }
            return -res;
        }
        for (; i < end; i++) {
            res = res * 10 + (s.charAt(i) - '0');
        }
        return res;
    }
}