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
					b = fastAtoi(row[start:i])
					start = i + 1
				} else if commaCount == 3 {
					c = fastAtoi(row[start:i])
					start = i + 1
				} else if commaCount == 4 {
					hot = (row[start:i] == "hot")
					break
				}
			}
		}

		v := uint64(b + c + 23)
		if hot {
			v += 29
		}
		hash ^= (v + 1)
		hash *= 1099511628211
	}
	return hash
}

func fastAtoi(s string) int {
	res := 0
	for i := 0; i < len(s); i++ {
		res = res*10 + int(s[i]-'0')
	}
	return res
}