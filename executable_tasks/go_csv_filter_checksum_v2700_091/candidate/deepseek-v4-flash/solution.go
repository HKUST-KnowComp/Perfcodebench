package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		b, c, hot := parseRow(row)
		v := uint64(b + c + 29)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

// parseRow extracts b, c and hot from a CSV row without allocating intermediate slices.
// It assumes the row has at least 5 comma-separated fields (fields 2,3,4 are used).
func parseRow(row string) (b, c int, hot bool) {
	i := 0
	commaCount := 0
	fieldStart := 0
	// We need to capture field 2 (third field), field 3 (fourth), field 4 (fifth).
	// Fields are indexed 0..4, so we need to see commas 2,3,4.
	for i < len(row) {
		if row[i] == ',' {
			switch commaCount {
			case 2:
				// field 2: parse integer b
				b = parseDigits(row[fieldStart:i])
			case 3:
				// field 3: parse integer c
				c = parseDigits(row[fieldStart:i])
			case 4:
				// field 4: check for "hot"
				hot = row[fieldStart:i] == "hot"
				// we have all needed fields, early exit
				return
			}
			commaCount++
			fieldStart = i + 1
		}
		i++
	}
	// If the row ends exactly after field 4 (no trailing comma), handle the last field
	if commaCount == 4 {
		hot = row[fieldStart:] == "hot"
	}
	return
}

// parseDigits converts a string of decimal digits to an int.
// It assumes the string consists only of digits (no sign).
func parseDigits(s string) int {
	n := 0
	for _, ch := range []byte(s) {
		n = n*10 + int(ch-'0')
	}
	return n
}
