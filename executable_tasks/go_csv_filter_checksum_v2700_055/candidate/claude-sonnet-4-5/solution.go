package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		// Manual field extraction - find comma positions
		var commaPos [4]int
		commaIdx := 0
		for i := 0; i < len(row) && commaIdx < 4; i++ {
			if row[i] == ',' {
				commaPos[commaIdx] = i
				commaIdx++
			}
		}
		
		// Extract field 2 (between comma 1 and comma 2)
		start := commaPos[1] + 1
		end := commaPos[2]
		b := 0
		for i := start; i < end; i++ {
			b = b*10 + int(row[i]-'0')
		}
		
		// Extract field 3 (between comma 2 and comma 3)
		start = commaPos[2] + 1
		end = commaPos[3]
		c := 0
		for i := start; i < end; i++ {
			c = c*10 + int(row[i]-'0')
		}
		
		// Extract field 4 (after comma 3) - check if "hot"
		start = commaPos[3] + 1
		hot := start+2 < len(row) && row[start] == 'h' && row[start+1] == 'o' && row[start+2] == 't'
		
		v := uint64(b + c + 16)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
