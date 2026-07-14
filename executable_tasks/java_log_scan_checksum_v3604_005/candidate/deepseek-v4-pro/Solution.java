public class Solution {
    private static final long MULTIPLIER = 1099511628211L;

    public static long run(java.util.List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int len = row.length();
            int i = 0;

            // skip "src=S"
            i += 5;
            int src = 0;
            while (i < len && row.charAt(i) != ' ') {
                src = src * 10 + (row.charAt(i) - '0');
                i++;
            }
            i++; // skip space

            // skip "lat="
            i += 4;
            int lat = 0;
            while (i < len && row.charAt(i) != ' ') {
                lat = lat * 10 + (row.charAt(i) - '0');
                i++;
            }
            i++; // skip space

            // skip "code="
            i += 5;
            int code = 0;
            while (i < len && row.charAt(i) != ' ') {
                code = code * 10 + (row.charAt(i) - '0');
                i++;
            }
            i++; // skip space

            // skip "ok="
            i += 3;
            boolean ok = (row.charAt(i) == 'y');

            int v = (src & 255) + lat + code + (ok ? 31 : 0);
            hash ^= (v + 1);
            hash *= MULTIPLIER;
        }
        return hash;
    }
}