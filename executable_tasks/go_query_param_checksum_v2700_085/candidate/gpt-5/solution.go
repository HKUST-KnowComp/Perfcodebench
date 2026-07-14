package main

func run(rows []string) uint64 {
	const (
		offset uint64 = 1469598103934665603
		prime  uint64 = 1099511628211
	)

	var hash uint64 = offset
	for _, row := range rows {
		s := row
		l := len(s)
		var u, score, flag int
		vi := 0
		i := 0
		for i < l && vi < 3 {
			c := s[i]
			if (c >= '0' && c <= '9') || c == '-' {
				neg := false
				if c == '-' {
					neg = true
					i++
					if i >= l {
						break
					}
				}
				val := 0
				for i < l {
					b := s[i]
					if b < '0' || b > '9' {
						break
					}
					val = val*10 + int(b-'0')
					i++
				}
				if neg {
					val = -val
				}
				switch vi {
				case 0:
					u = val
				case 1:
					score = val
				case 2:
					flag = val
				}
				vi++
			} else {
				i++
			}
		}

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= prime
	}
	return hash
}
