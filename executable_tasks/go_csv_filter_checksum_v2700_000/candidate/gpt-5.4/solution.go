package main

func run(rows []string) uint64 {
	const (
		offset64 = uint64(1469598103934665603)
		prime64  = uint64(1099511628211)
	)

	hash := offset64
	for _, row := range rows {
		field := 0
		idx := 0
		b := 0
		c := 0
		hot := false
		n := len(row)

		for i := 0; i <= n; i++ {
			if i != n && row[i] != ',' {
				continue
			}

			switch field {
			case 2, 3:
				val := 0
				for ; idx < i; idx++ {
					val = val*10 + int(row[idx]-'0')
				}
				if field == 2 {
					b = val
				} else {
					c = val
				}
			case 4:
				// Compare against "hot" without allocating.
				hot = i-idx == 3 && row[idx] == 'h' && row[idx+1] == 'o' && row[idx+2] == 't'
			}

			field++
			idx = i + 1
			if field > 4 {
				break
			}
		}

		v := uint64(b + c + 8)
		if hot {
			v += 29
		}
		hash ^= v
		hash *= prime64
	}
	return hash
}
