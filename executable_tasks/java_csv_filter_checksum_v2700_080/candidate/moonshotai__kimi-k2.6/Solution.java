import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (int i = 0, n = rows.size(); i < n; i++) {
            String row = rows.get(i);
            int len = row.length();
            int pos = 0;

            while (pos < len && row.charAt(pos) != ',') pos++;
            pos++;
            while (pos < len && row.charAt(pos) != ',') pos++;
            pos++;

            int start = pos;
            while (pos < len && row.charAt(pos) != ',') pos++;
            int b = parseInt(row, start, pos);
            pos++;

            start = pos;
            while (pos < len && row.charAt(pos) != ',') pos++;
            int c = parseInt(row, start, pos);
            pos++;

            start = pos;
            while (pos < len && row.charAt(pos) != ',') pos++;
            boolean hot = (pos - start == 3 && row.charAt(start) == 'h' && row.charAt(start + 1) == 'o' && row.charAt(start + 2) == 't');

            long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }

    private static int parseInt(String s, int start, int end) {
        if (start >= end) {
            throw new NumberFormatException();
        }
        boolean negative = false;
        char c = s.charAt(start);
        if (c == '-') {
            negative = true;
            start++;
        } else if (c == '+') {
            start++;
        }
        if (start >= end) {
            throw new NumberFormatException();
        }
        long val = 0;
        for (int i = start; i < end; i++) {
            int digit = s.charAt(i) - '0';
            if (digit < 0 || digit > 9) {
                throw new NumberFormatException();
            }
            val = val * 10 + digit;
            if (val > ((long) Integer.MAX_VALUE) + (negative ? 1L : 0L)) {
                throw new NumberFormatException();
            }
        }
        return negative ? -(int) val : (int) val;
    }
}
