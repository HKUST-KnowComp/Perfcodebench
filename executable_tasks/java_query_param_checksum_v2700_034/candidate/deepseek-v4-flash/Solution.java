import java.util.List;

public class Solution {
    private static int parseInt(String s, int start, int end) {
        int val = 0;
        for (int i = start; i < end; i++) {
            val = val * 10 + (s.charAt(i) - '0');
        }
        return val;
    }

    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        int size = rows.size();
        for (int i = 0; i < size; i++) {
            String row = rows.get(i);
            int len = row.length();
            int amp1 = row.indexOf('&');
            int u = parseInt(row, 2, amp1);
            int amp2 = row.indexOf('&', amp1 + 1);
            int s = parseInt(row, amp1 + 3, amp2);
            int f = parseInt(row, amp2 + 3, len);
            long v = (u & 2047) + s + f * 19L;
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}