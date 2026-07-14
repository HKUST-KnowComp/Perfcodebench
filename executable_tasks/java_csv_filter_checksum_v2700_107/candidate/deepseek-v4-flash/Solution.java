import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int len = row.length();
            int commaCount = 0;
            int secondComma = -1, thirdComma = -1, fourthComma = -1;
            for (int i = 0; i < len; i++) {
                char c = row.charAt(i);
                if (c == ',') {
                    commaCount++;
                    if (commaCount == 2) secondComma = i;
                    else if (commaCount == 3) thirdComma = i;
                    else if (commaCount == 4) { fourthComma = i; break; }
                }
            }
            int b = parseInt(row, secondComma + 1, thirdComma);
            int c = parseInt(row, thirdComma + 1, fourthComma);
            boolean hot = (len - (fourthComma + 1) == 3) &&
                          row.charAt(fourthComma + 1) == 'h' &&
                          row.charAt(fourthComma + 2) == 'o' &&
                          row.charAt(fourthComma + 3) == 't';
            long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }

    private static int parseInt(String s, int start, int end) {
        boolean negative = s.charAt(start) == '-';
        int i = negative ? start + 1 : start;
        int result = 0;
        while (i < end) {
            result = result * 10 + (s.charAt(i) - '0');
            i++;
        }
        return negative ? -result : result;
    }
}