package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		b := 0
		cVal := 0
		hot := false
		comma := 0
		val := 0
		inField := false

		for i := 0; i < len(row); i++ {
			ch := row[i]
			if ch == ',' {
				if inField {
					if comma == 2 {
						b = val
					} else if comma == 3 {
						cVal = val
					}
					val = 0
					inField = false
				}
				comma++
				if comma == 2 || comma == 3 {
					inField = true
				} else if comma == 4 {
					hot = i+4 <= len(row) && row[i+1] == 'h' && row[i+2] == 'o' && row[i+3] == 't' && (i+4 == len(row) || row[i+4] == ',')
					break
				}
			} else if inField {
				if ch >= '0' && ch <= '9' {
					val = val*10 + int(ch-'0')
				}
			}
		}
		if inField && comma == 3 {
			cVal = val
		}

		v := uint64(b + cVal + 20)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}