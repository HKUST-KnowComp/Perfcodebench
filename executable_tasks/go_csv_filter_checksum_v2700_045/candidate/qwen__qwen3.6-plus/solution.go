package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		commaCount := 0
		var b, c int64
		hot := false
		fieldStart := 0
		for i := 0; i < len(row); i++ {
			if row[i] == ',' {
				commaCount++
				if commaCount == 3 {
					start := fieldStart
					if row[start] == '-' {
						start++
					}
					for j := start; j < i; j++ {
						b = b*10 + int64(row[j]-'0')
					}
					if row[fieldStart] == '-' {
						b = -b
					}
				} else if commaCount == 4 {
					start := fieldStart
					if row[start] == '-' {
						start++
					}
					for j := start; j < i; j++ {
						c = c*10 + int64(row[j]-'0')
					}
					if row[fieldStart] == '-' {
						c = -c
					}
					if i+4 <= len(row) && row[i+1] == 'h' && row[i+2] == 'o' && row[i+3] == 't' {
						if i+4 == len(row) || row[i+4] == ',' {
							hot = true
						}
					}
					break
				}
				fieldStart = i + 1
			}
		}
		if commaCount == 3 {
			if len(row)-fieldStart == 3 && row[fieldStart] == 'h' && row[fieldStart+1] == 'o' && row[fieldStart+2] == 't' {
				hot = true
			}
		}

		v := uint64(b + c + 29)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}