package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		v := uint64(0)
		if matchRow(row) {
			v = uint64(row[2] + row[7])
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

func matchRow(s string) bool {
	if len(s) != 15 {
		return false
	}
	// Check two uppercase letters
	if s[0] < 'A' || s[0] > 'Z' || s[1] < 'A' || s[1] > 'Z' {
		return false
	}
	// Check four digits
	for i := 2; i < 6; i++ {
		if s[i] < '0' || s[i] > '9' {
			return false
		}
	}
	// Check hyphen
	if s[6] != '-' {
		return false
	}
	// Check eight hex digits (0-9, A-F)
	for i := 7; i < 15; i++ {
		c := s[i]
		if (c < '0' || c > '9') && (c < 'A' || c > 'F') {
			return false
		}
	}
	return true
}
