import java.util.List;

public class Solution {
    private static final long FNV_OFFSET = 1469598103934665603L;
    private static final long FNV_PRIME = 1099511628211L;

    public static long run(List<String> rows) {
        long hash = FNV_OFFSET;
        for (String row : rows) {
            int len = row.length();
            // Find positions of first four commas
            int comma1 = -1, comma2 = -1, comma3 = -1, comma4 = -1;
            int commaCount = 0;
            for (int i = 0; i < len && commaCount < 4; i++) {
                if (row.charAt(i) == ',') {
                    commaCount++;
                    if (commaCount == 1) comma1 = i;
                    else if (commaCount == 2) comma2 = i;
                    else if (commaCount == 3) comma3 = i;
                    else if (commaCount == 4) comma4 = i;
                }
            }
            // Parse b (third field)
            int b = parseInt(row, comma2 + 1, comma3);
            // Parse c (fourth field)
            int c = parseInt(row, comma3 + 1, comma4);
            // Check if fifth field is "hot"
            boolean hot = isHot(row, comma4 + 1, len);
            long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
            hash ^= (v + 1);
            hash *= FNV_PRIME;
        }
        return hash;
    }

    private static int parseInt(String s, int start, int end) {
        boolean negative = false;
        int i = start;
        if (i < end && s.charAt(i) == '-') {
            negative = true;
            i++;
        } else if (i < end && s.charAt(i) == '+') {
            i++;
        }
        int result = 0;
        while (i < end) {
            char c = s.charAt(i);
            if (c >= '0' && c <= '9') {
                result = result * 10 + (c - '0');
                i++;
            } else {
                break;
            }
        }
        return negative ? -result : result;
    }

    private static boolean isHot(String s, int start, int end) {
        return (end - start == 3) &&
               s.charAt(start) == 'h' &&
               s.charAt(start + 1) == 'o' &&
               s.charAt(start + 2) == 't';
    }
}