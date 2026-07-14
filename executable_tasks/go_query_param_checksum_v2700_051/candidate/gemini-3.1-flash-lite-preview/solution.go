package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var u, score, flag int
		var current int
		var val int
		var field int // 0: u, 1: score, 2: flag

		for i := 0; i < len(row); i++ {
			c := row[i]
			if c >= '0' && c <= '9' {
				val = val*10 + int(c-'0')
			} else if c == '=' {
				val = 0
			} else if c == '&' || i == len(row)-1 {
				if i == len(row)-1 {
					val = val*10 + int(c-'0')
				}
				switch field {
				case 0:
					u = val
				case 1:
					score = val
				case 2:
					flag = val
				}
				field++
				val = 0
			}
		}

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}