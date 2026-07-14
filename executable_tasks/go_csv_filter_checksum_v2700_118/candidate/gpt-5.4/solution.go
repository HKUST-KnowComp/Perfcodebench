package main

func run(rows []string) uint64 {
	const (
		offset64 = uint64(1469598103934665603)
		prime64  = uint64(1099511628211)
	)

	hash := offset64
	for _, row := range rows {
		field := 0
		val := 0
		b := 0
		c := 0
		hot := false

		start := 0
		n := len(row)
		for i := 0; i <= n; i++ {
			if i != n && row[i] != ',' {
				continue
			}

			if field == 2 || field == 3 {
				sign := 1
				j := start
				if j < i && row[j] == '-' {
					sign = -1
					j++
				}
				x := 0
				for ; j < i; j++ {
					x = x*10 + int(row[j]-'0')
				}
				x *= sign
				if field == 2 {
					b = x
				} else {
					c = x
				}
			} else if field == 4 {
				hot = i-start == 3 && row[start] == 'h' && row[start+1] == 'o' && row[start+2] == 't'
				break
			}

			field++
			start = i + 1
		}

		v := uint64(b + c + 10)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= prime64
	}
	return hash
}
