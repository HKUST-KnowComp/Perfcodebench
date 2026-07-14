package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		comma := 0
		var b, c uint64
		hot := false
		for i := 0; i < len(row); i++ {
			ch := row[i]
			if ch == ',' {
				comma++
			} else if comma == 2 {
				b = b*10 + uint64(ch-'0')
			} else if comma == 3 {
				c = c*10 + uint64(ch-'0')
			} else if comma == 4 {
				if ch == 'h' && i+2 < len(row) && row[i+1] == 'o' && row[i+2] == 't' {
					hot = true
				}
				break
			}
		}
		v := b + c + 24
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}