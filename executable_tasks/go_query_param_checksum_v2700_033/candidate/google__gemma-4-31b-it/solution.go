package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var u, score, flag int
		var current int
		var found int

		for i := 0; i < len(row); i++ {
			char := row[i]
			if char == '=' {
				found++
					i++ // skip to first digit
					continue
			}
			if char == '&' {
				switch found {
				case 1:
					u = current
				case 2:
					score = current
				}
				current = 0
				continue
			}
			if char >= '0' && char <= '9' {
				current = current*10 + int(char-'0')
			}
		}
		// Last parameter
		flag = current

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}