package main

func run(rows []string) uint64 {
	const (
		offset uint64 = 1469598103934665603
		prime  uint64 = 1099511628211
	)

	hash := offset
	for _, row := range rows {
		var u, score, flag int
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
		flag = val

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= prime
	}
	return hash
}
