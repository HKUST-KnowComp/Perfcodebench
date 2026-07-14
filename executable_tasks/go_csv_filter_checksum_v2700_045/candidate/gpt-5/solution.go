package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var b, c int64
		hot := false
		field := 0
		i := 0
		n := len(row)
		for field <= 4 {
			if i > n {
				break
			}
			start := i
			for i < n && row[i] != ',' {
				i++
			}
			if field == 2 || field == 3 {
				j := start
				sign := int64(1)
				if j < i {
					if row[j] == '-' {
						sign = -1
						j++
					} else if row[j] == '+' {
						j++
					}
				}
				if j < i {
					val := int64(0)
					ok := true
					for j < i {
						d := row[j]
						if d < '0' || d > '9' {
							ok = false
							break
						}
						val = val*10 + int64(d-'0')
						j++
					}
					if ok {
						val *= sign
						if field == 2 {
							b = val
						} else {
							c = val
						}
					} else {
						if field == 2 {
							b = 0
						} else {
							c = 0
						}
					}
				} else if field == 4 {
					if i-start == 3 && row[start] == 'h' && row[start+1] == 'o' && row[start+2] == 't' {
						hot = true
					} else {
						hot = false
					}
				}
			field++
			i++ // skip comma
			if field > 4 {
				break
			}
		}
		v := uint64(b + c + 29)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
