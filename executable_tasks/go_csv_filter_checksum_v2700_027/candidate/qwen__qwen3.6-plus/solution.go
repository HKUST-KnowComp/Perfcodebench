package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		commaCount := 0
		start := 0
		b := 0
		c := 0
		hot := false

		for i := 0; i < len(row); i++ {
			if row[i] == ',' {
				commaCount++
				if commaCount == 2 {
					start = i + 1
				} else if commaCount == 3 {
					for j := start; j < i; j++ {
						b = b*10 + int(row[j]-'0')
					}
					start = i + 1
				} else if commaCount == 4 {
					for j := start; j < i; j++ {
						c = c*10 + int(row[j]-'0')
					}
					start = i + 1
				} else if commaCount == 5 {
					if i-start == 3 && row[start] == 'h' && row[start+1] == 'o' && row[start+2] == 't' {
						hot = true
					}
					break
				}
			}
		}
		if commaCount == 4 {
			if len(row)-start == 3 && row[start] == 'h' && row[start+1] == 'o' && row[start+2] == 't' {
				hot = true
			}
		}

		v := uint64(b + c + 11)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}