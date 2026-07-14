import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int len = row.length();
            int commaCount = 0;
            int pos2 = -1;
            int pos3 = -1;
            int pos4 = -1;
            int pos5 = -1;

            // Single pass to find comma positions
            for (int i = 0; i < len; i++) {
                if (row.charAt(i) == ',') {
                    commaCount++;
                    if (commaCount == 2) pos2 = i + 1;
                    else if (commaCount == 3) pos3 = i + 1;
                    else if (commaCount == 4) pos4 = i + 1;
                    else if (commaCount == 5) {
                        pos5 = i;
                        break;
                    }
                }
            }
            
            // If the row doesn't have enough commas, handle gracefully (though task implies valid CSV)
            if (pos2 == -1 || pos3 == -1 || pos4 == -1) continue;
            if (pos5 == -1) pos5 = len;

            // Parse b (index 2)
            int b = 0;
            int end2 = findNextComma(row, pos2, len);
            for (int i = pos2; i < end2; i++) {
                b = b * 10 + (row.charAt(i) - '0');
            }

            // Parse c (index 3)
            int c = 0;
            int end3 = findNextComma(row, end2 + 1, len);
            for (int i = end2 + 1; i < end3; i++) {
                c = c * 10 + (row.charAt(i) - '0');
            }

            // Check hot (index 4)
            boolean hot = false;
            int end4 = findNextComma(row, end3 + 1, len);
            if (end4 == -1) end4 = len;
            
            // Check if parts[4] is "hot"
            if (end4 - (end3 + 1) == 3) {
                if (row.charAt(end3 + 1) == 'h' && 
                    row.charAt(end3 + 2) == 'o' && 
                    row.charAt(end3 + 3) == 't') {
                    hot = true;
                }
            }

            long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }

    private static int findNextComma(String s, int start, int len) {
        for (int i = start; i < len; i++) {
            if (s.charAt(i) == ',') return i;
        }
        return -1;
    }
}