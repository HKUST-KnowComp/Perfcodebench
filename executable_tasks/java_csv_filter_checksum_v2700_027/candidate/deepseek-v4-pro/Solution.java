import java.util.List;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int i1 = row.indexOf(',');
            int i2 = row.indexOf(',', i1 + 1);
            int i3 = row.indexOf(',', i2 + 1);
            int i4 = row.indexOf(',', i3 + 1);
            int i5 = row.indexOf(',', i4 + 1);

            // parse field 2 (b)
            int b = 0;
            int bStart = i2 + 1;
            int bEnd = i3;
            boolean bNeg = false;
            int bi = bStart;
            if (bi < bEnd && row.charAt(bi) == '-') {
                bNeg = true;
                bi++;
            }
            while (bi < bEnd) {
                b = b * 10 + (row.charAt(bi) - '0');
                bi++;
            }
            if (bNeg) b = -b;

            // parse field 3 (c)
            int c = 0;
            int cStart = i3 + 1;
            int cEnd = i4;
            boolean cNeg = false;
            int ci = cStart;
            if (ci < cEnd && row.charAt(ci) == '-') {
                cNeg = true;
                ci++;
            }
            while (ci < cEnd) {
                c = c * 10 + (row.charAt(ci) - '0');
                ci++;
            }
            if (cNeg) c = -c;

            // check field 4 (hot)
            int hStart = i4 + 1;
            int hEnd = (i5 != -1) ? i5 : row.length();
            boolean hot = (hEnd - hStart == 3) &&
                          row.charAt(hStart) == 'h' &&
                          row.charAt(hStart + 1) == 'o' &&
                          row.charAt(hStart + 2) == 't';

            long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}