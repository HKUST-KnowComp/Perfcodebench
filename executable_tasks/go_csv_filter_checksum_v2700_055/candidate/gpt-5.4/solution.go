package main

func run(rows []string) uint64 {
	const (
		offset64 = uint64(1469598103934665603)
		prime64  = uint64(1099511628211)
	)

	hash := offset64
	for _, row := range rows {
		field := 0
		numIdx := 0
		b, c := 0, 0
		hot := false
		start := 0

		for i := 0; i <= len(row); i++ {
			if i != len(row) && row[i] != ',' {
				continue
			}

			if field == 2 || field == 3 {
				n := 0
				sign := 1
				j := start
				if j < i && row[j] == '-' {
					sign = -1
					j++
				}
				for ; j < i; j++ {
					n = n*10 + int(row[j]-'0')
				}
				n *= sign
				if numIdx == 0 {
					b = n
					numIdx = 1
				} else {
					c = n
				}
			} else if field == 4 {
				hot = i-start == 3 && row[start] == 'h' && row[start+1] == 'o' && row[start+2] == 't'
				break
			}

			field++
			start = i + 1
		}

		v := uint64(b + c + 16)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= prime64
	}
	return hash
}
