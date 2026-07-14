package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		user := 0
		score := 0
		flag := false
		
		foundUser := false
		foundScore := false
		foundFlag := false
		
		i := 0
		n := len(row)
		
		for i < n {
			if foundUser && foundScore && foundFlag {
				break
			}
			
			// Look for "user":
			if !foundUser && i+7 < n && row[i] == '"' && row[i+1] == 'u' && row[i+2] == 's' && row[i+3] == 'e' && row[i+4] == 'r' && row[i+5] == '"' && row[i+6] == ':' {
				i += 7
				for i < n && (row[i] < '0' || row[i] > '9') {
					i++
				}
				for i < n && row[i] >= '0' && row[i] <= '9' {
					user = user*10 + int(row[i]-'0')
					i++
				}
				foundUser = true
				continue
			}
			
			// Look for "score":
			if !foundScore && i+8 < n && row[i] == '"' && row[i+1] == 's' && row[i+2] == 'c' && row[i+3] == 'o' && row[i+4] == 'r' && row[i+5] == 'e' && row[i+6] == '"' && row[i+7] == ':' {
				i += 8
				for i < n && (row[i] < '0' || row[i] > '9') {
					i++
				}
				for i < n && row[i] >= '0' && row[i] <= '9' {
					score = score*10 + int(row[i]-'0')
					i++
				}
				foundScore = true
				continue
			}
			
			// Look for "flag":true
			if !foundFlag && i+12 < n && row[i] == '"' && row[i+1] == 'f' && row[i+2] == 'l' && row[i+3] == 'a' && row[i+4] == 'g' && row[i+5] == '"' && row[i+6] == ':' && row[i+7] == 't' && row[i+8] == 'r' && row[i+9] == 'u' && row[i+10] == 'e' {
				flag = true
				foundFlag = true
				i += 11
				continue
			}
			
			i++
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