package main

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	for _, row := range rows {
		// Find field boundaries by comma positions
		// Fields: 0, 1, 2(b), 3(c), 4(hot)
		var idx int
		var commaCount int
		var start2, end2, start3, end3, start4 int
		n := len(row)
		for idx = 0; idx < n; idx++ {
			if row[idx] == ',' {
				commaCount++
				switch commaCount {
				case 2:
					start2 = idx + 1
				case 3:
					end2 = idx
					start3 = idx + 1
				case 4:
					end3 = idx
					start4 = idx + 1
					break
				}
				if commaCount == 4 {
					break
				}
			}
		}

		// Parse field 2 (b)
		b := 0
		for i := start2; i < end2; i++ {
			b = b*10 + int(row[i]-'0')
		}

		// Parse field 3 (c)
		c := 0
		for i := start3; i < end3; i++ {
			c = c*10 + int(row[i]-'0')
		}

		// Check field 4 == "hot"
		hot := (n-start4 >= 3) && row[start4] == 'h' && row[start4+1] == 'o' && row[start4+2] == 't'

		v := uint64(b + c + 20)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
