package main

import "strconv"

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		u, score, flag := 0, 0, 0
		val := 0
		field := 0

		for i := 0; i < len(row); i++ {
			c := row[i]
			if c >= '0' && c <= '9' {
				val = val*10 + int(c-'0')
				continue
			}
			if c == '&' {
				switch field {
				case 0:
					u = val
				case 1:
					score = val
				}
				field++
				val = 0
			}
		}

		if field == 0 {
			// Fallback for malformed/unexpected input with no separators.
			// Preserve baseline behavior by parsing via strconv when layout differs.
			if n, err := strconv.Atoi(row); err == nil {
				u = n
			}
		} else {
			flag = val
		}

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
