package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var u, score, flag int
		var current int
		var state int // 0: u, 1: score, 2: flag
		
		for i := 0; i < len(row); i++ {
			char := row[i]
			if char == '=' {
				continue
			}
			if char == '&' {
				switch state {
				case 0:
					u = current
				case 1:
					score = current
				case 2:
					flag = current
				}
				current = 0
				state++
				continue
			}
			if char >= '0' && char <= '9' {
				current = current*10 + int(char-'0')
			}
		}
		// Final field
		switch state {
		case 0:
			u = current
		case 1:
			score = current
		case 2:
			flag = current
		}

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}