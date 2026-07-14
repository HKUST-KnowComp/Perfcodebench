package main

func run(rows []string) uint64 {
	const offset uint64 = 1469598103934665603
	const prime  uint64 = 1099511628211
	hash := offset
	for _, row := range rows {
		var v uint64
		if isValid(row) {
			v = uint64(row[2] + row[7])
		}
		hash ^= v + 1
		hash *= prime
	}
	return hash
}

func isValid(s string) bool {
	if len(s) != 15 {
		return false
	}
	// positions 0-1: uppercase letters
	for i := 0; i < 2; i++ {
		if s[i] < 'A' || s[i] > 'Z' {
			return false
		}
	}
	// positions 2-5: digits
	for i := 2; i < 6; i++ {
		if s[i] < '0' || s[i] > '9' {
			return false
		}
	}
	// position 6: hyphen
	if s[6] != '-' {
		return false
	}
	// positions 7-14: hex digits (0-9, A-F)
	for i := 7; i < 15; i++ {
		c := s[i]
		if !((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')) {
			return false
		}
	}
	return true
}
