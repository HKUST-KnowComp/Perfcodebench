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
	out := make([]byte, n*12)
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		for i := 0; i < n; i++ {
			off := i * 12
			id := ids[i]
			out[off] = byte(id)
			out[off+1] = byte(id >> 8)
			out[off+2] = byte(id >> 16)
			out[off+3] = byte(id >> 24)

			t := ts[i]
			out[off+4] = byte(t)
			out[off+5] = byte(t >> 8)
			out[off+6] = byte(t >> 16)
			out[off+7] = byte(t >> 24)

			l := levels[i]
			out[off+8] = byte(l)
			out[off+9] = byte(l >> 8)

			ln := lens[i]
			out[off+10] = byte(ln)
			out[off+11] = byte(ln >> 8)
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}