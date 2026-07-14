package main

func matchRow(s string) bool {
	if len(s) != 15 {
		return false
	}

	b0 := s[0]
	if b0 < 'A' || b0 > 'Z' {
		return false
	}
	b1 := s[1]
	if b1 < 'A' || b1 > 'Z' {
		return false
	}

	b2 := s[2]
	if b2 < '0' || b2 > '9' {
		return false
	}
	b3 := s[3]
	if b3 < '0' || b3 > '9' {
		return false
	}
	b4 := s[4]
	if b4 < '0' || b4 > '9' {
		return false
	}
	b5 := s[5]
	if b5 < '0' || b5 > '9' {
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
		var v uint64
		if matchRow(row) {
			v = uint64(row[2] + row[7])
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
