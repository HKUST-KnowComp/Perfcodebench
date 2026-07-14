package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		v := uint64(0)
		if matchPattern(row) {
			v = uint64(row[2] + row[7])
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

func matchPattern(s string) bool {
	if len(s) != 15 {
		return false
	}
	// [A-Z] at positions 0, 1
	c := s[0]
	if c < 'A' || c > 'Z' {
		return false
	}
	c = s[1]
	if c < 'A' || c > 'Z' {
		return false
	}
	// [0-9] at positions 2, 3, 4, 5
	c = s[2]
	if c < '0' || c > '9' {
		return false
	}
	c = s[3]
	if c < '0' || c > '9' {
		return false
	}
	c = s[4]
	if c < '0' || c > '9' {
		return false
	}
	c = s[5]
	if c < '0' || c > '9' {
		return false
	}
	// '-' at position 6
	if s[6] != '-' {
		return false
	}
	// [0-9A-F] at positions 7-14
	c = s[7]
	if !((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')) {
		return false
	}
	c = s[8]
	if !((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')) {
		return false
	}
	c = s[9]
	if !((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')) {
		return false
	}
	c = s[10]
	if !((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')) {
		return false
	}
	c = s[11]
	if !((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')) {
		return false
	}
	c = s[12]
	if !((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')) {
		return false
	}
	c = s[13]
	if !((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')) {
		return false
	}
	c = s[14]
	if !((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')) {
		return false
	}
	return true
}