package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		n := len(row)
		i := 0
		commas := 0

		for i < n && commas < 2 {
			if row[i] == ',' {
				commas++
			}
			i++
		}

		b := 0
		neg := false
		if i < n && (row[i] == '-' || row[i] == '+') {
			if row[i] == '-' {
				neg = true
			}
			i++
		}
		for i < n && row[i] >= '0' && row[i] <= '9' {
			b = b*10 + int(row[i]-'0')
			i++
		}
		if neg {
			b = -b
		}
		if i < n {
			i++
		}

		c := 0
		neg = false
		if i < n && (row[i] == '-' || row[i] == '+') {
			if row[i] == '-' {
				neg = true
			}
			i++
		}
		for i < n && row[i] >= '0' && row[i] <= '9' {
			c = c*10 + int(row[i]-'0')
			i++
		}
		if neg {
			c = -c
		}
		if i < n {
			i++
		}

		hot := i+3 <= n && row[i] == 'h' && row[i+1] == 'o' && row[i+2] == 't' && (i+3 == n || row[i+3] == ',')

		v := uint64(b + c + 11)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}