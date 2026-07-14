package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var u, score, flag int
		val := 0
		field := 0
		inNum := false

		for i := 0; i < len(row); i++ {
			c := row[i]
			if c >= '0' && c <= '9' {
				val = val*10 + int(c-'0')
				inNum = true
			} else {
				if inNum {
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
					inNum = false
					if field == 3 {
						break
					}
				}
			}
		}

		if inNum && field < 3 {
			switch field {
			case 0:
				u = val
			case 1:
				score = val
			case 2:
				flag = val
			}
		}

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
