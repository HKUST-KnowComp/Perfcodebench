import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int len = row.length();
            int field = 0;
            int fieldStart = 0;
            int b = 0, c = 0;
            boolean hot = false;
            for (int i = 0; i <= len; i++) {
                char ch;
                if (i == len) {
                    ch = ','; // force end-of-field processing
                } else {
                    ch = row.charAt(i);
                }
                if (ch == ',') {
                    // process field
                    switch (field) {
                        case 2:
                            b = parseInteger(row, fieldStart, i);
                            break;
                        case 3:
                            c = parseInteger(row, fieldStart, i);
                            break;
                        case 4:
                            hot = isHot(row, fieldStart, i);
                            // after field 4 we have all we need
                            i = len; // break outer loop
                            break;
                    }
                    fieldStart = i + 1;
                    field++;
                    if (field > 4) {
                        break;
                    }
                }
            }
            long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }

    private static int parseInteger(String s, int start, int end) {
        if (start >= end) return 0;
        boolean negative = false;
        int i = start;
        char first = s.charAt(i);
        if (first == '-') {
            negative = true;
            i++;
        } else if (first == '+') {
            i++;
        }
        int value = 0;
        while (i < end) {
            char c = s.charAt(i);
            value = value * 10 + (c - '0');
            i++;
        }
        return negative ? -value : value;
    }

    private static boolean isHot(String s, int start, int end) {
        int len = end - start;
        if (len != 3) return false;
        return s.charAt(start) == 'h' && s.charAt(start + 1) == 'o' && s.charAt(start + 2) == 't';
    }
}