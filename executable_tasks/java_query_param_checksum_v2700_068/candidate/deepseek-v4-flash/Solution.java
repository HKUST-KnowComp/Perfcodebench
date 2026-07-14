import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int len = row.length();
            char[] chars = row.toCharArray();

            // Find positions of the two '&' characters
            int amp1 = -1, amp2 = -1;
            for (int i = 0; i < len; i++) {
                if (chars[i] == '&') {
                    if (amp1 == -1) {
                        amp1 = i;
                    } else {
                        amp2 = i;
                        break;
                    }
                }
            }

            // Parse u: substring(2) of part0 (skip "u=")
            int uStart = 2;
            int uEnd = amp1;
            int u = parseInt(chars, uStart, uEnd);

            // Parse s: substring(6) of part1 -> start = amp1 + 1 + 6, end = amp2
            int sStart = amp1 + 1 + 6;
            int sEnd = amp2;
            int s = parseInt(chars, sStart, sEnd);

            // Parse f: substring(5) of part2 -> start = amp2 + 1 + 5, end = len
            int fStart = amp2 + 1 + 5;
            int fEnd = len;
            int f = parseInt(chars, fStart, fEnd);

            long v = (u & 2047) + s + f * 19L;
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }

    private static int parseInt(char[] chars, int start, int end) {
        int result = 0;
        for (int i = start; i < end; i++) {
            char c = chars[i];
            result = result * 10 + (c - '0');
        }
        return result;
    }
}