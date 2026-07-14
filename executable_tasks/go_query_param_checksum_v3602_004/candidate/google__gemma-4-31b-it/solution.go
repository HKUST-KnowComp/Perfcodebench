package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var u, score, flag int
		var current int
		var state int // 0: looking for u, 1: reading u, 2: looking for score, 3: reading score, 4: looking for flag, 5: reading flag

		for i := 0; i < len(row); i++ {
			b := row[i]
			if state == 0 {
				if b == 'u' && i+1 < len(row) && row[i+1] == '=' {
					i += 1
					state = 1
				} else if b == '&' {
					state = 2
			}
			} else if state == 1 {
				if b >= '0' && b <= '9' {
					current = current*10 + int(b-'0')
			} else if b == '&' {
					u = current
					current = 0
					state = 2
			}
			} else if state == 2 {
				if b == 's' && i+2 < len(row) && row[i+1] == 'c' && row[i+2] == 'o' && row[i+3] == 'r' && row[i+4] == 'e' && row[i+5] == '=' {
					i += 6
					state = 3
				} else if b == '&' {
					state = 4
			}
			} else if state == 3 {
				if b >= '0' && b <= '9' {
					current = current*10 + int(b-'0')
			} else if b == '&' {
					score = current
					current = 0
					state = 4
			}
			} else if state == 4 {
				if b == 'f' && i+3 < len(row) && row[i+1] == 'l' && row[i+2] == 'a' && row[i+3] == 'g' && row[i+4] == '=' {
					i += 5
					state = 5
				} else if b == '&' {
					state = 4
			}
			} else if state == 5 {
				if b >= '0' && b <= '9' {
					current = current*10 + int(b-'0')
			} else {
					flag = current
					break
			}
		}
		if state == 5 {
			flag = current
		}

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}