package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var u, score, flag int
		var n int
		var sign int = 1
		field := 0

		// Skip "u="
		i := 2
		for ; i < len(row); i++ {
			c := row[i]
			if c == '&' {
				u = sign * n
				field = 1
				n = 0
				sign = 1
				i += 7 // skip "&score="
				break
			}
			if c == '-' {
				sign = -1
			} else {
				n = n*10 + int(c-'0')
			}
		}

		if field == 1 {
			for ; i < len(row); i++ {
				c := row[i]
				if c == '&' {
					score = sign * n
					field = 2
					n = 0
					sign = 1
					i += 6 // skip "&flag="
					break
				}
				if c == '-' {
					sign = -1
				} else {
					n = n*10 + int(c-'0')
				}
			}
		}

		if field == 2 {
			for ; i < len(row); i++ {
				c := row[i]
				if c == '-' {
					sign = -1
				} else {
					n = n*10 + int(c-'0')
				}
			}
			flag = sign * n
		}

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
