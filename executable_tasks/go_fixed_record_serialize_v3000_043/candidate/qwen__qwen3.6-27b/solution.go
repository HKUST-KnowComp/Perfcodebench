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
	buf := make([]byte, n*12)
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		for i := 0; i < n; i++ {
			idx := i * 12
			v := ids[i]
			buf[idx] = byte(v)
			buf[idx+1] = byte(v >> 8)
			buf[idx+2] = byte(v >> 16)
			buf[idx+3] = byte(v >> 24)
			v = ts[i]
			buf[idx+4] = byte(v)
			buf[idx+5] = byte(v >> 8)
			buf[idx+6] = byte(v >> 16)
			buf[idx+7] = byte(v >> 24)
			v16 := levels[i]
			buf[idx+8] = byte(v16)
			buf[idx+9] = byte(v16 >> 8)
			v16 = lens[i]
			buf[idx+10] = byte(v16)
			buf[idx+11] = byte(v16 >> 8)
		}
		hash = solutionChecksumBytes(buf)
	}
	return hash
}