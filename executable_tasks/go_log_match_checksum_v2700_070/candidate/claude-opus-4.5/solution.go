package main

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	for _, row := range rows {
		v := uint64(0)
		if len(row) == 15 && isMatch(row) {
			v = uint64(row[2]) + uint64(row[7])
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

func isMatch(s string) bool {
	// [A-Z]{2}
	c := s[0]
	if c < 'A' || c > 'Z' {
		return false
	}
	c = s[1]
	if c < 'A' || c > 'Z' {
		return false
	}
	// [0-9]{4}
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
	// -
	if s[6] != '-' {
		return false
	}
	// [0-9A-F]{8}
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