package main

func run(rows []string) uint64 {
	const (
		offset64 = uint64(1469598103934665603)
		prime64  = uint64(1099511628211)
	)

	hash := offset64
	for _, row := range rows {
		field := 0
		i := 0
		n := len(row)
		b, c := 0, 0
		hot := false

		for i < n && field < 5 {
			start := i
			for i < n && row[i] != ',' {
				i++
			}

			switch field {
			case 2:
				v := 0
				if start < i {
					s := row[start:i]
					sign := 1
					j := 0
					if s[0] == '-' {
						sign = -1
						j = 1
					} else if s[0] == '+' {
						j = 1
					}
					for ; j < len(s); j++ {
						v = v*10 + int(s[j]-'0')
					}
					v *= sign
				}
				b = v
			case 3:
				v := 0
				if start < i {
					s := row[start:i]
					sign := 1
					j := 0
					if s[0] == '-' {
						sign = -1
						j = 1
					} else if s[0] == '+' {
						j = 1
					}
					for ; j < len(s); j++ {
						v = v*10 + int(s[j]-'0')
					}
					v *= sign
				}
				c = v
			case 4:
				hot = i-start == 3 && row[start] == 'h' && row[start+1] == 'o' && row[start+2] == 't'
			}

			field++
			if i < n && row[i] == ',' {
				i++
			}
		}

		v := uint64(b + c + 29)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= prime64
	}
	return hash
}
