import java.util.List;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int c1 = row.indexOf(',');
            int c2 = row.indexOf(',', c1 + 1);
            int c3 = row.indexOf(',', c2 + 1);
            int c4 = row.indexOf(',', c3 + 1);
            int c5 = row.indexOf(',', c4 + 1);
            int end4 = (c5 < 0) ? row.length() : c5;

            int b = 0;
            int sb = c2 + 1;
            if (row.charAt(sb) == '-') {
                sb++;
                for (int j = sb; j < c3; j++) b = b * 10 + (row.charAt(j) - '0');
                b = -b;
            } else {
                for (int j = sb; j < c3; j++) b = b * 10 + (row.charAt(j) - '0');
            }

            int c = 0;
            int sc = c3 + 1;
            if (row.charAt(sc) == '-') {
                sc++;
                for (int j = sc; j < c4; j++) c = c * 10 + (row.charAt(j) - '0');
                c = -c;
            } else {
                for (int j = sc; j < c4; j++) c = c * 10 + (row.charAt(j) - '0');
            }

            boolean hot = (end4 - c4 == 4) &&
                          row.charAt(c4 + 1) == 'h' &&
                          row.charAt(c4 + 2) == 'o' &&
                          row.charAt(c4 + 3) == 't';

            long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}