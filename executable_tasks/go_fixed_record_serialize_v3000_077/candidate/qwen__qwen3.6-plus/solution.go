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
		off := 0
		for i := 0; i < n; i++ {
			id := ids[i]
			buf[off] = byte(id)
			buf[off+1] = byte(id >> 8)
			buf[off+2] = byte(id >> 16)
			buf[off+3] = byte(id >> 24)
			off += 4

			t := ts[i]
			buf[off] = byte(t)
			buf[off+1] = byte(t >> 8)
			buf[off+2] = byte(t >> 16)
			buf[off+3] = byte(t >> 24)
			off += 4

			l := levels[i]
			buf[off] = byte(l)
			buf[off+1] = byte(l >> 8)
			off += 2

			ln := lens[i]
			buf[off] = byte(ln)
			buf[off+1] = byte(ln >> 8)
			off += 2
		}
		hash = solutionChecksumBytes(buf)
	}
	return hash
}