package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for i := 0; i < len(rows); i++ {
		row := rows[i]
		var v uint64
		if len(row) == 15 {
			b0 := row[0]
			b1 := row[1]
			if b0 >= 'A' && b0 <= 'Z' && b1 >= 'A' && b1 <= 'Z' {
				d2 := row[2]
				d3 := row[3]
				d4 := row[4]
				d5 := row[5]
				if d2 >= '0' && d2 <= '9' && d3 >= '0' && d3 <= '9' && d4 >= '0' && d4 <= '9' && d5 >= '0' && d5 <= '9' && row[6] == '-' {
					ok := true
					for j := 7; j < 15; j++ {
						c := row[j]
						if !((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')) {
							ok = false
							break
						}
					}
					if ok {
						v = uint64(row[2] + row[7])
					}
				}
			}
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
