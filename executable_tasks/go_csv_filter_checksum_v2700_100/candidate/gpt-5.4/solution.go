package main

func run(rows []string) uint64 {
	const (
		offset64 = uint64(1469598103934665603)
		prime64  = uint64(1099511628211)
	)

	hash := offset64
	for _, row := range rows {
		field := 0
		i := 0
		n := len(row)
		b := 0
		c := 0
		hot := false

		for i < n {
			start := i
			for i < n && row[i] != ',' {
				i++
			}

			switch field {
			case 2:
				v := 0
				for j := start; j < i; j++ {
					v = v*10 + int(row[j]-'0')
				}
				b = v
			case 3:
				v := 0
				for j := start; j < i; j++ {
					v = v*10 + int(row[j]-'0')
				}
				c = v
			case 4:
				hot = i-start == 3 && row[start] == 'h' && row[start+1] == 'o' && row[start+2] == 't'
			}

			field++
			i++
		}

		v := uint64(b + c + 15)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= prime64
	}
	return hash
}
