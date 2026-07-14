package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		i := 0
		for c := 0; c < 2; c++ {
			for i < len(row) && row[i] != ',' {
				i++
			}
			i++
		}

		b := 0
		for ; i < len(row) && row[i] >= '0' && row[i] <= '9'; i++ {
			b = b*10 + int(row[i]-'0')
		}
		i++

		c := 0
		for ; i < len(row) && row[i] >= '0' && row[i] <= '9'; i++ {
			c = c*10 + int(row[i]-'0')
		}
		i++

		hot := len(row) > i+2 && row[i] == 'h' && row[i+1] == 'o' && row[i+2] == 't'

		v := uint64(b + c + 16)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
