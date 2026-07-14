package main

func solutionChecksumBytes(data []byte) uint64 {
	var hash uint64 = 1469598103934665603
	for _, b := range data {
		hash ^= uint64(b)
		hash *= 1099511628211
	}
	return hash
}

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	n := len(ids)
	if n == 0 {
		return solutionChecksumBytes(nil)
	}
	out := make([]byte, n*12)
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		pos := 0
		for i := 0; i < n; i++ {
			v := ids[i]
			out[pos] = byte(v)
			pos++
			out[pos] = byte(v >> 8)
			pos++
			out[pos] = byte(v >> 16)
			pos++
			out[pos] = byte(v >> 24)
			pos++

			v = ts[i]
			out[pos] = byte(v)
			pos++
			out[pos] = byte(v >> 8)
			pos++
			out[pos] = byte(v >> 16)
			pos++
			out[pos] = byte(v >> 24)
			pos++

			v = levels[i]
			out[pos] = byte(v)
			pos++
			out[pos] = byte(v >> 8)
			pos++

			v = lens[i]
			out[pos] = byte(v)
			pos++
			out[pos] = byte(v >> 8)
			pos++
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}