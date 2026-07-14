package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var u, score, flag int
		var n int
		var sign int = 1
		field := 0

		for i := 0; i < len(row); i++ {
			c := row[i]
			if c == '=' {
				n = 0
				sign = 1
				j := i + 1
				if j < len(row) && row[j] == '-' {
					sign = -1
					j++
				}
				for ; j < len(row); j++ {
					d := row[j]
					if d == '&' {
						break
					}
					n = n*10 + int(d-'0')
				}
				n *= sign
				switch field {
				case 0:
					u = n
				case 1:
					score = n
				case 2:
					flag = n
				}
				field++
				i = j
			}
		}

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
