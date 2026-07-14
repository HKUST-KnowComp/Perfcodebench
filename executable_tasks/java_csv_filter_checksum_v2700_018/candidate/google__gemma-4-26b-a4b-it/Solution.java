import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int len = row.length();
            int commaCount = 0;
            int start = 0;
            
            int b = 0;
            int c = 0;
            boolean hot = false;

            // Manual scan to find indices and parse values in one pass
            for (int i = 0; i <= len; i++) {
                if (i == len || row.charAt(i) == ',') {
                    commaCount++;
                    if (commaCount == 3) {
                        // Parse b (index 2)
                        int valB = 0;
                        for (int j = start; j < i; j++) {
                            valB = valB * 10 + (row.charAt(j) - '0');
                        }
                        b = valB;
                    } else if (commaCount == 4) {
                        // Parse c (index 3)
                        int valC = 0;
                        for (int j = start; j < i; j++) {
                            valC = valC * 10 + (row.charAt(j) - '0');
                        }
                        c = valC;
                    } else if (commaCount == 5) {
                        // Check hot (index 4)
                        // We check if the substring from start to i is "hot"
                        if (i - start == 3 && 
                            row.charAt(start) == 'h' && 
                            row.charAt(start + 1) == 'o' && 
                            row.charAt(start + 2) == 't') {
                            hot = true;
                        }
                    }
                    start = i + 1;
                }
            }

            long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}