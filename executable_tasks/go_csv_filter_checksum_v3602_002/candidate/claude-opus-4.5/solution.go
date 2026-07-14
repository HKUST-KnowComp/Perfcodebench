package main

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	for _, row := range rows {
		// Find field boundaries by locating commas
		// Fields: 0, 1, 2(b), 3(c), 4(hot)
		var commaIdx [5]int
		commaCount := 0
		for i := 0; i < len(row) && commaCount < 5; i++ {
			if row[i] == ',' {
				commaIdx[commaCount] = i
				commaCount++
			}
		}

		// Parse field 2 (b): between comma[1] and comma[2]
		b := 0
		for i := commaIdx[1] + 1; i < commaIdx[2]; i++ {
			b = b*10 + int(row[i]-'0')
		}

		// Parse field 3 (c): between comma[2] and comma[3]
		c := 0
		for i := commaIdx[2] + 1; i < commaIdx[3]; i++ {
			c = c*10 + int(row[i]-'0')
		}

		// Check field 4: starts at comma[3]+1
		// "hot" check: length 3 and matches
		start4 := commaIdx[3] + 1
		end4 := len(row)
		if commaCount > 4 {
			end4 = commaIdx[4]
		}
		hot := (end4-start4 == 3) && row[start4] == 'h' && row[start4+1] == 'o' && row[start4+2] == 't'

		v := uint64(b + c + 11)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}