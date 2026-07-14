package main

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	for _, row := range rows {
		// Find field boundaries by scanning for commas
		// Fields: 0, 1, 2(b), 3(c), 4(hot)
		var commaIdx [4]int
		commaCount := 0
		for i := 0; i < len(row) && commaCount < 4; i++ {
			if row[i] == ',' {
				commaIdx[commaCount] = i
				commaCount++
			}
		}

		// Parse field 2 (b): between commaIdx[1]+1 and commaIdx[2]
		b := 0
		for i := commaIdx[1] + 1; i < commaIdx[2]; i++ {
			b = b*10 + int(row[i]-'0')
		}

		// Parse field 3 (c): between commaIdx[2]+1 and commaIdx[3]
		c := 0
		for i := commaIdx[2] + 1; i < commaIdx[3]; i++ {
			c = c*10 + int(row[i]-'0')
		}

		// Check field 4 (hot): starts at commaIdx[3]+1
		// "hot" is 3 chars, check if it matches
		start := commaIdx[3] + 1
		hot := len(row)-start >= 3 && row[start] == 'h' && row[start+1] == 'o' && row[start+2] == 't'

		v := uint64(b + c + 20)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
