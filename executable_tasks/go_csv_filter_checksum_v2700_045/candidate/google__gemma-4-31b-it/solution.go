package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	const prime uint64 = 1099511628211

	for _, row := range rows {
		var b, c int
		var hot bool
		commaCount := 0
		start := 0

		for i := 0; i < len(row); i++ {
			if row[i] == ',' {
				commaCount++
				
				switch commaCount {
				case 3:
					// Parse field 2 (index 2)
					for j := start; j < i; j++ {
						b = b*10 + int(row[j]-'0')
					}
				case 4:
					// Parse field 3 (index 3)
					for j := start; j < i; j++ {
						c = c*10 + int(row[j]-'0')
					}
				case 5:
					// Check field 4 (index 4) for "hot"
					if i-start == 3 && row[start] == 'h' && row[start+1] == 'o' && row[start+2] == 't' {
						hot = true
					}
				}
				start = i + 1
			}
		}
		
		// Handle the last field if it's the one we need
		if commaCount == 4 {
			// Last field is index 4
			if len(row)-start == 3 && row[start] == 'h' && row[start+1] == 'o' && row[start+2] == 't' {
				hot = true
			}
		}

		v := uint64(b + c + 29)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= prime
	}
	return hash
}