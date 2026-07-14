package main

func matchRow(s string) bool {
	if len(s) != 15 {
		return false
	}

	b := s

	c0 := b[0]
	if c0 < 'A' || c0 > 'Z' {
		return false
	}
	c1 := b[1]
	if c1 < 'A' || c1 > 'Z' {
		return false
	}

	c2 := b[2]
	if c2 < '0' || c2 > '9' {
		return false
	}
	c3 := b[3]
	if c3 < '0' || c3 > '9' {
		return false
	}
	c4 := b[4]
	if c4 < '0' || c4 > '9' {
		return false
	}
	c5 := b[5]
	if c5 < '0' || c5 > '9' {
		return false
	}

	if b[6] != '-' {
		return false
	}

	c7 := b[7]
	if !isHexUpper(c7) {
		return false
	}
	c8 := b[8]
	if !isHexUpper(c8) {
		return false
	}
	c9 := b[9]
	if !isHexUpper(c9) {
		return false
	}
	c10 := b[10]
	if !isHexUpper(c10) {
		return false
	}
	c11 := b[11]
	if !isHexUpper(c11) {
		return false
	}
	c12 := b[12]
	if !isHexUpper(c12) {
		return false
	}
	c13 := b[13]
	if !isHexUpper(c13) {
		return false
	}
	c14 := b[14]
	if !isHexUpper(c14) {
		return false
	}

	return true
}

func isHexUpper(c byte) bool {
	return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')
}

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
