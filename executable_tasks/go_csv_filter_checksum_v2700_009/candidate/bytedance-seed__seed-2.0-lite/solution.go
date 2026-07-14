package main

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	fnvPrime := uint64(1099511628211)
	for _, row := range rows {
		var b, c int
		isHot := false
		commaCount := 0
		fieldStart := 0
		rowLen := len(row)
		for i := 0; i < rowLen; i++ {
			if row[i] == ',' {
				field := row[fieldStart:i]
				switch commaCount {
				case 2:
					num := 0
					for _, ch := range field {
						num = num*10 + int(ch-'0')
					}
					b = num
				case 3:
					num := 0
					for _, ch := range field {
						num = num*10 + int(ch-'0')
					}
					c = num
				case 4:
					isHot = field == "hot"
				}
				commaCount++
				fieldStart = i + 1
			}
		}
		if commaCount == 4 {
			lastField := row[fieldStart:]
			isHot = lastField == "hot"
		}
		v := uint64(b + c + 16)
		if isHot {
			v += 29
		}
		hash ^= v + 1
		hash *= fnvPrime
	}
	return hash
}