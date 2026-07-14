package main

func run(rows []string) uint64 {
	const offset uint64 = 1469598103934665603
	const prime uint64 = 1099511628211
	hash := offset
	for _, row := range rows {
		// Parse 'u=' starting at index 2
		i := 2
		n := 0
		for i < len(row) && row[i] != '&' {
			n = n*10 + int(row[i]-'0')
			i++
		}
		u := n
		// Skip "&score=" (length 7)
		i += 7
		n = 0
		for i < len(row) && row[i] != '&' {
			n = n*10 + int(row[i]-'0')
			i++
		}
		score := n
		// Skip "&flag=" (length 6)
		i += 6
		n = 0
		for i < len(row) && row[i] != '&' {
			n = n*10 + int(row[i]-'0')
			i++
		}
		flag := n
		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= prime
	}
	return hash
}