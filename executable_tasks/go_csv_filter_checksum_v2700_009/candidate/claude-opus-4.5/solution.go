package main

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	for _, row := range rows {
		// Find comma positions for fields 2, 3, 4 (0-indexed)
		// Format: f0,f1,f2,f3,f4,...
		var c1, c2, c3, c4 int = -1, -1, -1, -1
		count := 0
		for i := 0; i < len(row); i++ {
			if row[i] == ',' {
				count++
				switch count {
				case 1:
					c1 = i
				case 2:
					c2 = i
				case 3:
					c3 = i
				case 4:
					c4 = i
					goto parse
				}
			}
		}
	parse:
		// parts[2] is between c2 and c3
		// parts[3] is between c3 and c4
		// parts[4] starts at c4+1
		b := parseIntFast(row, c2+1, c3)
		c := parseIntFast(row, c3+1, c4)

		// Check if parts[4] == "hot"
		// parts[4] starts at c4+1, check if it starts with "hot" and is exactly 3 chars or followed by comma
		hot := false
		start4 := c4 + 1
		if start4+3 <= len(row) {
			if row[start4] == 'h' && row[start4+1] == 'o' && row[start4+2] == 't' {
				if start4+3 == len(row) || row[start4+3] == ',' {
					hot = true
				}
			}
		}

		v := uint64(b + c + 16)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

func parseIntFast(s string, start, end int) int {
	if start >= end {
		return 0
	}
	neg := false
	if s[start] == '-' {
		neg = true
		start++
	}
	n := 0
	for i := start; i < end; i++ {
		n = n*10 + int(s[i]-'0')
	}
	if neg {
		return -n
	}
	return n
}