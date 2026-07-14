package main

func run(rows []string) uint64 {
	const prime = 1099511628211
	hash := uint64(1469598103934665603)
	for _, row := range rows {
		user := 0
		score := 0
		flag := false
		i := 0
		n := len(row)
		for i < n {
			// skip to next '"'
			for i < n && row[i] != '"' {
				i++
			}
			if i >= n {
				break
			}
			// key start after '"'
			keyStart := i + 1
			i++ // skip opening quote
			// read key until closing '"'
			for i < n && row[i] != '"' {
				i++
			}
			if i >= n {
				break
			}
			keyEnd := i
			i++ // skip closing quote
			// expect ':'
			if i < n && row[i] == ':' {
				i++
			}
			// value start
			valStart := i
			for i < n && row[i] != ',' {
				i++
			}
			valEnd := i
			// skip comma if present
			if i < n && row[i] == ',' {
				i++
			}
			// determine key using slice comparison (zero allocation, shares backing array)
			key := row[keyStart:keyEnd]
			val := row[valStart:valEnd]
			switch key {
			case "user":
				for _, b := range val {
					if b >= '0' && b <= '9' {
						user = user*10 + int(b-'0')
					}
				}
			case "score":
				for _, b := range val {
					if b >= '0' && b <= '9' {
						score = score*10 + int(b-'0')
					}
				}
			case "flag":
				if val == "true" {
					flag = true
				}
			}
		}
		v := uint64((user & 1023) + score)
		if flag {
			v += 17
		}
		hash ^= v + 1
		hash *= prime
	}
	return hash
}
