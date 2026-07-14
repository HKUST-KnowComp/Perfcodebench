package main

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	for _, row := range rows {
		// Find comma positions manually to avoid allocation from Split
		i0 := indexByte(row, ',', 0)
		i1 := indexByte(row, ',', i0+1)
		i2 := indexByte(row, ',', i1+1)
		i3 := indexByte(row, ',', i2+1)

		// Parse field 2 (between i1 and i2)
		b := parseIntFast(row, i1+1, i2)
		// Parse field 3 (between i2 and i3)
		c := parseIntFast(row, i2+1, i3)

		v := uint64(b + c + 29)

		// Check field 4 for "hot" (starts at i3+1)
		// "hot" is 3 chars; next char is either comma or end
		if i3+4 <= len(row) && row[i3+1] == 'h' && row[i3+2] == 'o' && row[i3+3] == 't' {
			if i3+4 == len(row) || row[i3+4] == ',' {
				v += 29
			}
		}

		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

// indexByte finds the next ',' starting at position start.
// Returns the index of the comma, or len(s) if not found.
func indexByte(s string, c byte, start int) int {
	for i := start; i < len(s); i++ {
		if s[i] == c {
			return i
		}
	}
	return len(s)
}

// parseIntFast parses a non-negative integer from s[start:end].
// Assumes valid input (digits only, no sign).
func parseIntFast(s string, start, end int) int {
	n := 0
	for i := start; i < end; i++ {
		n = n*10 + int(s[i]-'0')
	}
	return n
}
