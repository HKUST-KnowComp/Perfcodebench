package main

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	for _, row := range rows {
		// Find comma positions manually to avoid allocation
		var c1, c2, c3, c4 int
		commaCount := 0
		for i := 0; i < len(row); i++ {
			if row[i] == ',' {
				commaCount++
				switch commaCount {
				case 1:
					c1 = i
				case 2:
					c2 = i
				case 3:
					c3 = i
				case 4:
					c4 = i
					goto done
				}
			}
		}
	done:
		// Parse parts[2] (between c2 and c3)
		b := 0
		for i := c2 + 1; i < c3; i++ {
			b = b*10 + int(row[i]-'0')
		}
		// Parse parts[3] (between c3 and c4)
		c := 0
		for i := c3 + 1; i < c4; i++ {
			c = c*10 + int(row[i]-'0')
		}
		// Check parts[4] == "hot" (starts at c4+1)
		hot := len(row) >= c4+4 && row[c4+1] == 'h' && row[c4+2] == 'o' && row[c4+3] == 't'
		if len(row) > c4+4 && row[c4+4] != ',' {
			hot = false
		}
		v := uint64(b + c + 7)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}