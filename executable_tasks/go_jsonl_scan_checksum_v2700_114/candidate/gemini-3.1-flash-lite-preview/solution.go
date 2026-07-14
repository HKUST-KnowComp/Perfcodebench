package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var user, score int
		var flag bool
		
		// Manual scan to avoid allocations and multiple passes
		for i := 0; i < len(row); i++ {
			if row[i] == '"' {
				if i+7 < len(row) && row[i+1] == 'u' && row[i+2] == 's' && row[i+3] == 'e' && row[i+4] == 'r' {
					i += 7
					for i < len(row) && (row[i] < '0' || row[i] > '9') { i++ }
					for i < len(row) && row[i] >= '0' && row[i] <= '9' {
						user = user*10 + int(row[i]-'0')
						i++
					}
				} else if i+8 < len(row) && row[i+1] == 's' && row[i+2] == 'c' && row[i+3] == 'o' && row[i+4] == 'r' && row[i+5] == 'e' {
					i += 8
					for i < len(row) && (row[i] < '0' || row[i] > '9') { i++ }
					for i < len(row) && row[i] >= '0' && row[i] <= '9' {
						score = score*10 + int(row[i]-'0')
						i++
					}
				} else if i+7 < len(row) && row[i+1] == 'f' && row[i+2] == 'l' && row[i+3] == 'a' && row[i+4] == 'g' {
					i += 7
					if i+4 < len(row) && row[i] == 't' && row[i+1] == 'r' && row[i+2] == 'u' && row[i+3] == 'e' {
						flag = true
					}
				}
			}
		}
		
		v := uint64((user & 1023) + score)
		if flag {
			v += 17
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}