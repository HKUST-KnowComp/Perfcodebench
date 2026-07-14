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
	if !((c7 >= '0' && c7 <= '9') || (c7 >= 'A' && c7 <= 'F')) {
		return false
	}
	c8 := b[8]
	if !((c8 >= '0' && c8 <= '9') || (c8 >= 'A' && c8 <= 'F')) {
		return false
	}
	c9 := b[9]
	if !((c9 >= '0' && c9 <= '9') || (c9 >= 'A' && c9 <= 'F')) {
		return false
	}
	c10 := b[10]
	if !((c10 >= '0' && c10 <= '9') || (c10 >= 'A' && c10 <= 'F')) {
		return false
	}
	c11 := b[11]
	if !((c11 >= '0' && c11 <= '9') || (c11 >= 'A' && c11 <= 'F')) {
		return false
	}
	c12 := b[12]
	if !((c12 >= '0' && c12 <= '9') || (c12 >= 'A' && c12 <= 'F')) {
		return false
	}
	c13 := b[13]
	if !((c13 >= '0' && c13 <= '9') || (c13 >= 'A' && c13 <= 'F')) {
		return false
	}
	c14 := b[14]
	if !((c14 >= '0' && c14 <= '9') || (c14 >= 'A' && c14 <= 'F')) {
		return false
	}

	return true
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
