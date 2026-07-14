import java.util.List;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int len = row.length();
            int amp1 = row.indexOf('&');
            int amp2 = row.indexOf('&', amp1 + 1);
            int u = parseInt(row, 2, amp1);
            int s = parseInt(row, amp1 + 1 + 6, amp2);
            int f = parseInt(row, amp2 + 1 + 5, len);
            long v = (u & 2047) + s + f * 19L;
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }

    private static int parseInt(String s, int start, int end) {
        int result = 0;
        int i = start;
        boolean negative = false;
        if (i < end) {
            char c = s.charAt(i);
            if (c == '-') {
                negative = true;
                i++;
            } else if (c == '+') {
                i++;
            }
        }
        while (i < end) {
            result = result * 10 + (s.charAt(i) - '0');
            i++;
        }
        return negative ? -result : result;
    }
}