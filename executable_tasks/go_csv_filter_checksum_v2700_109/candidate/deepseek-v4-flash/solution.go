package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		i := 0
		field := 0
		var b, c int
		var hot bool
		for {
			j := i
			for j < len(row) && row[j] != ',' {
				j++
			}
			switch field {
			case 2:
				// convert slice to int
				b = 0
				for k := i; k < j; k++ {
					b = b*10 + int(row[k]-'0')
				}
			case 3:
				c = 0
				for k := i; k < j; k++ {
					c = c*10 + int(row[k]-'0')
				}
			case 4:
				hot = (j-i == 3 && row[i] == 'h' && row[i+1] == 'o' && row[i+2] == 't')
			}
			if j == len(row) {
				break
			}
			field++
			i = j + 1
		}
		v := uint64(b + c + 24)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
