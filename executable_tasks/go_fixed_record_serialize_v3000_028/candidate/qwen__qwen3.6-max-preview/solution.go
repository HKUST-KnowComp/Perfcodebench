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
			off := i * 12
			v := ids[i]
			buf[off] = byte(v)
			buf[off+1] = byte(v >> 8)
			buf[off+2] = byte(v >> 16)
			buf[off+3] = byte(v >> 24)

			v = ts[i]
			buf[off+4] = byte(v)
			buf[off+5] = byte(v >> 8)
			buf[off+6] = byte(v >> 16)
			buf[off+7] = byte(v >> 24)

			l := levels[i]
			buf[off+8] = byte(l)
			buf[off+9] = byte(l >> 8)

			l = lens[i]
			buf[off+10] = byte(l)
			buf[off+11] = byte(l >> 8)
		}
		hash = solutionChecksumBytes(buf)
	}
	return hash
}