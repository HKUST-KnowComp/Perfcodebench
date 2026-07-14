package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		idx := 0
		for i := 0; i < 2; i++ {
			for idx < len(row) && row[idx] != ',' {
				idx++
			}
			idx++
		}

		var b uint64
		for idx < len(row) && row[idx] != ',' {
			b = b*10 + uint64(row[idx]-'0')
			idx++
		}
		idx++

		var c uint64
		for idx < len(row) && row[idx] != ',' {
			c = c*10 + uint64(row[idx]-'0')
			idx++
		}
		idx++

		hot := idx+3 <= len(row) && row[idx] == 'h' && row[idx+1] == 'o' && row[idx+2] == 't' && (idx+3 == len(row) || row[idx+3] == ',')

		v := b + c + 25
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}