package main

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	for _, row := range rows {
		// Find comma positions for fields 2, 3, 4 (0-indexed)
		// Format: f0,f1,f2,f3,f4,...
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
					goto parse
				}
			}
		}
	parse:
		// Parse field 2 (between c1 and c2)
		b := 0
		for i := c1 + 1; i < c2; i++ {
			b = b*10 + int(row[i]-'0')
		}
		// Parse field 3 (between c2 and c3)
		c := 0
		for i := c2 + 1; i < c3; i++ {
			c = c*10 + int(row[i]-'0')
		}
		// Check field 4 for "hot" (between c3 and c4)
		// "hot" is 3 chars
		hot := (c4-c3 == 4) && row[c3+1] == 'h' && row[c3+2] == 'o' && row[c3+3] == 't'

		v := uint64(b + c + 10)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}