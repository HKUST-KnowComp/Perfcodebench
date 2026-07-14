import java.util.List;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int i1 = row.indexOf(',');
            int i2 = row.indexOf(',', i1 + 1);
            int i3 = row.indexOf(',', i2 + 1);
            int i4 = row.indexOf(',', i3 + 1);

            // parse b
            int b = 0;
            boolean negB = false;
            int idx = i2 + 1;
            if (row.charAt(idx) == '-') { negB = true; idx++; }
            while (idx < i3) {
                b = b * 10 + (row.charAt(idx) - '0');
                idx++;
            }
            if (negB) b = -b;

            // parse c
            int c = 0;
            boolean negC = false;
            idx = i3 + 1;
            if (row.charAt(idx) == '-') { negC = true; idx++; }
            while (idx < i4) {
                c = c * 10 + (row.charAt(idx) - '0');
                idx++;
            }
            if (negC) c = -c;

            // check hot
            int hotStart = i4 + 1;
            int i5 = row.indexOf(',', hotStart);
            int hotEnd = (i5 == -1) ? row.length() : i5;
            boolean hot = (hotEnd - hotStart == 3 &&
                           row.charAt(hotStart) == 'h' &&
                           row.charAt(hotStart + 1) == 'o' &&
                           row.charAt(hotStart + 2) == 't');

            long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}