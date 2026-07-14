package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var user, score int
		var flag bool
		
		// Manual scan for fields to avoid allocations
		for i := 0; i < len(row); i++ {
			if row[i] == '"' {
				if i+7 < len(row) && row[i+1:i+6] == "user" {
					i += 7
					for i < len(row) && (row[i] < '0' || row[i] > '9') { i++ }
					for i < len(row) && row[i] >= '0' && row[i] <= '9' {
						user = user*10 + int(row[i]-'0')
						i++
					}
				} else if i+8 < len(row) && row[i+1:i+7] == "score" {
					i += 8
					for i < len(row) && (row[i] < '0' || row[i] > '9') { i++ }
					for i < len(row) && row[i] >= '0' && row[i] <= '9' {
						score = score*10 + int(row[i]-'0')
						i++
					}
				} else if i+6 < len(row) && row[i+1:i+6] == "flag" {
					i += 6
					for i < len(row) && row[i] != ':' { i++ }
					if i+4 < len(row) && row[i+1:i+5] == "true" {
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