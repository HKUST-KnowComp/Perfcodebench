package main

func match(s string) bool {
	if len(s) != 15 {
		return false
	}
	if s[0] < 'A' || s[0] > 'Z' || s[1] < 'A' || s[1] > 'Z' {
		return false
	}
	if s[2] < '0' || s[2] > '9' || s[3] < '0' || s[3] > '9' || s[4] < '0' || s[4] > '9' || s[5] < '0' || s[5] > '9' {
		return false
	}
	if s[6] != '-' {
		return false
	}
	for i := 7; i < 15; i++ {
		c := s[i]
		if !((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')) {
			return false
		}
	}
	return true
}

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		v := uint64(0)
		if match(row) {
			v = uint64(row[2] + row[7])
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}