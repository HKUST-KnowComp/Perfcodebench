package main

func run(rows []string) uint64 {
	const (
		offset64 = uint64(1469598103934665603)
		prime64  = uint64(1099511628211)
	)

	hash := offset64
	for _, row := range rows {
		u, score, flag := 0, 0, 0
		val := 0
		field := 0 // 0=u, 1=score, 2=flag

		// Parse digits directly, avoiding Split/TrimPrefix/Atoi allocations and overhead.
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
		hash *= prime64
	}
	return hash
}
