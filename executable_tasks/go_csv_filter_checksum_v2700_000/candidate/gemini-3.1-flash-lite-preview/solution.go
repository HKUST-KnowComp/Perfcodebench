package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var b, c, commaCount int
		var start int
		var hot bool

		for i := 0; i < len(row); i++ {
			if row[i] == ',' {
				commaCount++
				if commaCount == 2 {
					val := 0
					for j := start; j < i; j++ {
						val = val*10 + int(row[j]-'0')
					}
					b = val
					start = i + 1
				} else if commaCount == 3 {
					val := 0
					for j := start; j < i; j++ {
						val = val*10 + int(row[j]-'0')
					}
					c = val
					start = i + 1
				} else if commaCount == 4 {
					hot = (i-start == 3 && row[start] == 'h' && row[start+1] == 'o' && row[start+2] == 't')
					break
				}
			}
		}

		v := uint64(b + c + 7)
		if hot {
			v += 29
		}
		hash ^= (v + 1)
		hash *= 1099511628211
	}
	return hash
}