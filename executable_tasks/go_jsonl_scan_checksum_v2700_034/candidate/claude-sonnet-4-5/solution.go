package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		user := 0
		score := 0
		flag := false
		
		i := 0
		n := len(row)
		
		for i < n {
			// Skip to next field
			for i < n && row[i] != '"' {
				i++
			}
			if i >= n {
				break
			}
			i++ // skip opening quote
			
			// Check field name
			if i+4 < n && row[i] == 'u' && row[i+1] == 's' && row[i+2] == 'e' && row[i+3] == 'r' {
				// Find colon and parse number
				for i < n && row[i] != ':' {
					i++
				}
				i++
				for i < n && (row[i] < '0' || row[i] > '9') {
					i++
				}
				for i < n && row[i] >= '0' && row[i] <= '9' {
					user = user*10 + int(row[i]-'0')
					i++
				}
			} else if i+5 < n && row[i] == 's' && row[i+1] == 'c' && row[i+2] == 'o' && row[i+3] == 'r' && row[i+4] == 'e' {
				// Find colon and parse number
				for i < n && row[i] != ':' {
					i++
				}
				i++
				for i < n && (row[i] < '0' || row[i] > '9') {
					i++
				}
				for i < n && row[i] >= '0' && row[i] <= '9' {
					score = score*10 + int(row[i]-'0')
					i++
				}
			} else if i+4 < n && row[i] == 'f' && row[i+1] == 'l' && row[i+2] == 'a' && row[i+3] == 'g' {
				// Check if value is true
				for i < n && row[i] != ':' {
					i++
				}
				i++
				for i < n && row[i] == ' ' {
					i++
				}
				if i+3 < n && row[i] == 't' && row[i+1] == 'r' && row[i+2] == 'u' && row[i+3] == 'e' {
					flag = true
				}
				i++
			} else {
				i++
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
