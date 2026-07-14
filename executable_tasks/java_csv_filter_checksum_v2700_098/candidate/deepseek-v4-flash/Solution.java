import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int len = row.length();
            int fieldIndex = 0;
            int fieldStart = 0;
            int b = 0, c = 0;
            boolean hot = false;
            for (int i = 0; i <= len; i++) {
                if (i == len || row.charAt(i) == ',') {
                    if (fieldIndex == 2) {
                        b = parseInt(row, fieldStart, i);
                    } else if (fieldIndex == 3) {
                        c = parseInt(row, fieldStart, i);
                    } else if (fieldIndex == 4) {
                        hot = (i - fieldStart == 3) 
                           && row.charAt(fieldStart) == 'h' 
                           && row.charAt(fieldStart + 1) == 'o' 
                           && row.charAt(fieldStart + 2) == 't';
                    }
                    fieldIndex++;
                    fieldStart = i + 1;
                }
            }
            long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }

    private static int parseInt(String s, int start, int end) {
        int val = 0;
        boolean negative = false;
        if (s.charAt(start) == '-') {
            negative = true;
            start++;
        }
        for (int i = start; i < end; i++) {
            val = val * 10 + (s.charAt(i) - '0');
        }
        return negative ? -val : val;
    }
}