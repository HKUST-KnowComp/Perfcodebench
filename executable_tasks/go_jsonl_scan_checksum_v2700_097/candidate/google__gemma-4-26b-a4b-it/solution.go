package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		user := 0
		score := 0
		flag := false
		
		// Single pass scan to find keys and values
		i := 0
		n := len(row)
		for i < n {
			// Look for the start of a key (quote)
			for i < n && row[i] != '"' {
				i++
			}
			if i >= n {
				break
			}
			
			// Identify key
			start := i
			i++ // skip opening quote
			for i < n && row[i] != '"' {
				i++
			}
			key := row[start:i+1]
			i++ // skip closing quote
			
			// Skip to value
			for i < n && row[i] != ':' {
				i++
			}
			if i < n {
				i++ // skip colon
			}
			
			// Skip whitespace/delimiters after colon
			for i < n && (row[i] == ' ' || row[i] == '\t' || row[i] == '"' || row[i] == ':') {
				i++
			}

			if len(key) >= 6 && key[len(key)-6:] == "\"user\"" {
				for i < n && (row[i] < '0' || row[i] > '9') {
					i++
				}
				for i < n && row[i] >= '0' && row[i] <= '9' {
					user = user*10 + int(row[i]-'0')
					i++
				}
			} else if len(key) >= 7 && key[len(key)-7:] == "\"score\"" {
				for i < n && (row[i] < '0' || row[i] > '9') {
					i++
				}
				for i < n && row[i] >= '0' && row[i] <= '9' {
					score = score*10 + int(row[i]-'0')
					i++
				}
			} else if len(key) >= 6 && key[len(key)-6:] == "\"flag\"" {
				// Check for :true
				foundTrue := false
				for j := i; j < n && j < i+10; j++ {
					if row[j] == 't' {
						if j+3 < n && row[j:j+4] == "true" {
							foundTrue = true
							break
						}
					}
				}
				if foundTrue {
					flag = true
				}
				// Advance i past the value to avoid re-scanning
				for i < n && row[i] != ',' && row[i] != '}' {
					i++
				}
			}
			
			// Move to next part
			for i < n && row[i] != ',' && row[i] != '}' {
				i++
			}
			if i < n && row[i] == ',' {
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