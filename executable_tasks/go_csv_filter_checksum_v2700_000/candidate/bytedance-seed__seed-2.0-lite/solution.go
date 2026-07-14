package main

func run(rows []string) uint64 {
	const fnvOffset = 1469598103934665603
	const fnvPrime = 1099511628211
	hash := fnvOffset

	for _, row := range rows {
		commaCount := 0
		fieldStart := 0
		var b, c int
		var isHot bool

		for i, r := range row {
			if r == ',' {
				switch commaCount {
				case 2:
					// Parse parts[2] to integer b
					b = 0
					for j := fieldStart; j < i; j++ {
						b = b*10 + int(row[j]-'0')
					}
				case 3:
					// Parse parts[3] to integer c
					c = 0
					for j := fieldStart; j < i; j++ {
						c = c*10 + int(row[j]-'0')
					}
				case 4:
					// Check if parts[4] is "hot"
					isHot = len(row)-fieldStart >= 3 && row[fieldStart] == 'h' && row[fieldStart+1] == 'o' && row[fieldStart+2] == 't'
					goto endRowScan // Exit early, no more fields needed
				}
				fieldStart = i + 1
				commaCount++
			}
		}
	endRowScan:
		// Maintain exact original calculation logic
		v := uint64(b + c + 7)
		if isHot {
			v += 29
		}
		hash ^= v + 1
		hash *= fnvPrime
	}
	return hash
}