import java.util.List;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int idx = row.indexOf(',');
            idx = row.indexOf(',', idx + 1);
            
            int bStart = idx + 1;
            idx = row.indexOf(',', bStart);
            int b = fastParseInt(row, bStart, idx);
            
            int cStart = idx + 1;
            idx = row.indexOf(',', cStart);
            int c = fastParseInt(row, cStart, idx);
            
            int hStart = idx + 1;
            int len = row.length();
            boolean hot = (hStart + 2 < len) &&
                          row.charAt(hStart) == 'h' &&
                          row.charAt(hStart + 1) == 'o' &&
                          row.charAt(hStart + 2) == 't' &&
                          (hStart + 3 == len || row.charAt(hStart + 3) == ',');
            
            long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }

    private static int fastParseInt(String s, int start, int end) {
        int res = 0;
        boolean neg = false;
        if (start < end) {
            char c0 = s.charAt(start);
            if (c0 == '-') {
                neg = true;
                start++;
            } else if (c0 == '+') {
                start++;
            }
        }
        for (int i = start; i < end; i++) {
            res = res * 10 + (s.charAt(i) - '0');
        }
        return neg ? -res : res;
    }
}