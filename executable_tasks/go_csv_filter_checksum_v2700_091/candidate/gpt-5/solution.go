package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		s := row
		n := len(s)
		prev := 0
		field := 0
		var b, c int
		var isHot bool

		for j := 0; ; j++ {
			if j == n || s[j] == ',' {
				if field == 2 { // third field
					i := prev
					sign := 1
					if i < j {
						if s[i] == '-' {
							sign = -1
							i++
						} else if s[i] == '+' {
							i++
						}
					}
					val := 0
					for i < j {
						val = val*10 + int(s[i]-'0')
						i++
					}
					b = sign * val
				} else if field == 3 { // fourth field
					i := prev
					sign := 1
					if i < j {
						if s[i] == '-' {
							sign = -1
							i++
						} else if s[i] == '+' {
							i++
						}
					}
					val := 0
					for i < j {
						val = val*10 + int(s[i]-'0')
						i++
					}
					c = sign * val
				} else if field == 4 { // fifth field
					if j-prev == 3 {
						if s[prev] == 'h' && s[prev+1] == 'o' && s[prev+2] == 't' {
							isHot = true
						}
					}
					// We only need up to the 5th field; stop scanning further.
					break
				}

				field++
				if j == n {
					break
				}
				prev = j + 1
			}

		v := uint64(b + c + 29)
		if isHot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
