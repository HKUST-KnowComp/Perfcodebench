package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var v uint64
		if len(row) == 15 {
			b0 := row[0]
			b1 := row[1]
			if b0 >= 'A' && b0 <= 'Z' && b1 >= 'A' && b1 <= 'Z' {
				if row[6] == '-' {
					b2 := row[2]
					b3 := row[3]
					b4 := row[4]
					b5 := row[5]
					if b2 >= '0' && b2 <= '9' && b3 >= '0' && b3 <= '9' && b4 >= '0' && b4 <= '9' && b5 >= '0' && b5 <= '9' {
						ok := true
						for i := 7; i < 15; i++ {
							c := row[i]
							if !((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')) {
								ok = false
								break
							}
						}
						if ok {
							v = uint64(b2 + row[7])
						}
					}
				}
			}
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
