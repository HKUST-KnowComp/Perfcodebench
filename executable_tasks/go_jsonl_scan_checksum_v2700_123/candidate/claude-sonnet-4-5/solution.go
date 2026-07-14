package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		user := 0
		score := 0
		flag := false
		
		start := 0
		for i := 0; i < len(row); i++ {
			if row[i] == ',' || i == len(row)-1 {
				end := i
				if i == len(row)-1 {
					end = len(row)
				}
				part := row[start:end]
				start = i + 1
				
				if len(part) < 7 {
					continue
				}
				
				if part[0] == '"' && part[1] == 'u' && part[2] == 's' && part[3] == 'e' && part[4] == 'r' {
					for j := 6; j < len(part); j++ {
						c := part[j]
						if c >= '0' && c <= '9' {
							user = user*10 + int(c-'0')
						}
					}
				} else if part[0] == '"' && part[1] == 's' && part[2] == 'c' && part[3] == 'o' && part[4] == 'r' && part[5] == 'e' {
					for j := 7; j < len(part); j++ {
						c := part[j]
						if c >= '0' && c <= '9' {
							score = score*10 + int(c-'0')
						}
					}
				} else if part[0] == '"' && part[1] == 'f' && part[2] == 'l' && part[3] == 'a' && part[4] == 'g' {
					for j := 5; j < len(part); j++ {
						if part[j] == 't' && j+3 < len(part) && part[j+1] == 'r' && part[j+2] == 'u' && part[j+3] == 'e' {
							flag = true
							break
						}
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
