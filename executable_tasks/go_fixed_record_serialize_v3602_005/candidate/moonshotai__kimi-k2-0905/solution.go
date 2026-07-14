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
	buf := make([]byte, 0, n*12) // avoid realloc
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		buf = buf[:0]
		off := 0
		buf = buf[:cap(buf)]
		for i := 0; i < n; i++ {
			id := ids[i]
			t := ts[i]
			lv := levels[i]
			ln := lens[i]
			buf[off] = byte(id)
			buf[off+1] = byte(id >> 8)
			buf[off+2] = byte(id >> 16)
			buf[off+3] = byte(id >> 24)
			buf[off+4] = byte(t)
			buf[off+5] = byte(t >> 8)
			buf[off+6] = byte(t >> 16)
			buf[off+7] = byte(t >> 24)
			buf[off+8] = byte(lv)
			buf[off+9] = byte(lv >> 8)
			buf[off+10] = byte(ln)
			buf[off+11] = byte(ln >> 8)
			off += 12
		}
		hash = solutionChecksumBytes(buf[:off])
	}
	return hash
}