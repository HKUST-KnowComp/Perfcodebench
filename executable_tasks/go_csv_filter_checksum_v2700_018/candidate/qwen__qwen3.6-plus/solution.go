package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var b, c uint64
		var hot bool
		comma := 0
		start := 0
		for i := 0; i < len(row); i++ {
			if row[i] == ',' {
				comma++
				if comma == 2 {
					start = i + 1
				} else if comma == 3 {
					for j := start; j < i; j++ {
						b = b*10 + uint64(row[j]-'0')
					}
					start = i + 1
				} else if comma == 4 {
					for j := start; j < i; j++ {
						c = c*10 + uint64(row[j]-'0')
					}
					if i+4 <= len(row) && row[i+1] == 'h' && row[i+2] == 'o' && row[i+3] == 't' {
						if i+4 == len(row) || row[i+4] == ',' {
							hot = true
						}
					}
					break
				}
			}
		}
		v := b + c + 25
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}