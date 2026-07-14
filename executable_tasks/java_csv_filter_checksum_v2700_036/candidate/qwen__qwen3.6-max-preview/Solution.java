import java.util.List;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int len = row.length();
            int c1 = row.indexOf(',');
            int c2 = row.indexOf(',', c1 + 1);
            int c3 = row.indexOf(',', c2 + 1);
            int c4 = row.indexOf(',', c3 + 1);
            int c5 = row.indexOf(',', c4 + 1);
            if (c5 == -1) c5 = len;

            int b = 0;
            int startB = c2 + 1;
            boolean negB = startB < c3 && row.charAt(startB) == '-';
            for (int i = negB ? startB + 1 : startB; i < c3; i++) {
                b = b * 10 + (row.charAt(i) - '0');
            }
            if (negB) b = -b;

            int c = 0;
            int startC = c3 + 1;
            boolean negC = startC < c4 && row.charAt(startC) == '-';
            for (int i = negC ? startC + 1 : startC; i < c4; i++) {
                c = c * 10 + (row.charAt(i) - '0');
            }
            if (negC) c = -c;

            boolean hot = (c5 - c4 == 4) && row.charAt(c4 + 1) == 'h' && row.charAt(c4 + 2) == 'o' && row.charAt(c4 + 3) == 't';

            long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}