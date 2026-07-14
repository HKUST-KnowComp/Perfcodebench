package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		v := uint64(0)
		if len(row) == 15 && isMatch(row) {
			v = uint64(row[2] + row[7])
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

func isMatch(s string) bool {
	// [A-Z]{2}
	if s[0] < 'A' || s[0] > 'Z' {
		return false
	}
	if s[1] < 'A' || s[1] > 'Z' {
		return false
	}
	// [0-9]{4}
	if s[2] < '0' || s[2] > '9' {
		return false
	}
	if s[3] < '0' || s[3] > '9' {
		return false
	}
	if s[4] < '0' || s[4] > '9' {
		return false
	}
	if s[5] < '0' || s[5] > '9' {
		return false
	}
	// -
	if s[6] != '-' {
		return false
	}
	// [0-9A-F]{8}
	if !isHex(s[7]) {
		return false
	}
	if !isHex(s[8]) {
		return false
	}
	if !isHex(s[9]) {
		return false
	}
	if !isHex(s[10]) {
		return false
	}
	if !isHex(s[11]) {
		return false
	}
	if !isHex(s[12]) {
		return false
	}
	if !isHex(s[13]) {
		return false
	}
	if !isHex(s[14]) {
		return false
	}
	return true
}

func isHex(c byte) bool {
	return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')
}
