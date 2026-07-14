package main

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	for _, row := range rows {
		// find 5 comma indices in one pass
		p0 := indexOf(row, 0, ',')
		p1 := indexOf(row, p0+1, ',')
		p2 := indexOf(row, p1+1, ',')
		p3 := indexOf(row, p2+1, ',')
		p4 := indexOf(row, p3+1, ',')

		b := atoi(row[p2:p3])
		c := atoi(row[p3:p4])
		v := uint64(b+c+20)
		// hot flag is exactly the byte at p4+1 == 'h' (no need to compare whole string)
		if row[p4+1] == 'h' {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

// fast ASCII-to-int for non-negative numbers
func atoi(s string) int {
	n := 0
	for i := 0; i < len(s); i++ {
		n = n*10 + int(s[i]-'0')
	}
	return n
}

// return index of sep starting from off; caller guarantees sep exists
func indexOf(s string, off int, sep byte) int {
	for i := off; i < len(s); i++ {
		if s[i] == sep {
			return i
		}
	}
	return -1
}