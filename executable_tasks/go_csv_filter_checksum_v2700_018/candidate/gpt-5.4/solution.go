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

		for i := 0; i <= len(row); i++ {
			if i != len(row) && row[i] != ',' {
				continue
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
				v := uint64(b + c + 25)
				if hot {
					v += 29
				}
				hash ^= v + 1
				hash *= prime64
				goto nextRow
			}

			field++
			start = i + 1
			_ = val
		}

	nextRow:
	}
	return hash
}
