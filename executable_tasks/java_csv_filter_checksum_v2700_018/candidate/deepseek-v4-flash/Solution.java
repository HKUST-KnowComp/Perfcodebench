import java.util.List;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int len = row.length();
            int commaCount = 0;
            int start2 = -1, end2 = -1, start3 = -1, end3 = -1, start4 = -1, end4 = -1;
            for (int i = 0; i < len; i++) {
                if (row.charAt(i) == ',') {
                    commaCount++;
                    if (commaCount == 2) {
                        start2 = i + 1;
                    } else if (commaCount == 3) {
                        end2 = i;
                        start3 = i + 1;
                    } else if (commaCount == 4) {
                        end3 = i;
                        start4 = i + 1;
                    } else if (commaCount == 5) {
                        end4 = i;
                        break;
                    }
                }
            }
            // If row ends before 5th comma, end4 = len
            if (end4 == -1) {
                end4 = len;
            }
            // Parse field2 as int
            int b = 0;
            boolean negative = false;
            int pos = start2;
            if (row.charAt(pos) == '-') { negative = true; pos++; }
            while (pos < end2) {
                b = b * 10 + (row.charAt(pos) - '0');
                pos++;
            }
            if (negative) b = -b;
            // Parse field3 as int
            int c = 0;
            negative = false;
            pos = start3;
            if (row.charAt(pos) == '-') { negative = true; pos++; }
            while (pos < end3) {
                c = c * 10 + (row.charAt(pos) - '0');
                pos++;
            }
            if (negative) c = -c;
            // Check field4 == "hot"
            boolean hot = (end4 - start4 == 3)
                && row.charAt(start4) == 'h'
                && row.charAt(start4+1) == 'o'
                && row.charAt(start4+2) == 't';
            long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}