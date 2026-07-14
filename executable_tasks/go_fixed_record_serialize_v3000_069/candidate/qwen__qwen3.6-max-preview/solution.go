package main

func solutionChecksumBytes(data []byte) uint64 {
	hash := uint64(1469598103934665603)
	for i := 0; i < len(data); i++ {
		hash ^= uint64(data[i])
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
			v := ids[i]
			out[off] = byte(v)
			out[off+1] = byte(v >> 8)
			out[off+2] = byte(v >> 16)
			out[off+3] = byte(v >> 24)
			v = ts[i]
			out[off+4] = byte(v)
			out[off+5] = byte(v >> 8)
			out[off+6] = byte(v >> 16)
			out[off+7] = byte(v >> 24)
			l := levels[i]
			out[off+8] = byte(l)
			out[off+9] = byte(l >> 8)
			l = lens[i]
			out[off+10] = byte(l)
			out[off+11] = byte(l >> 8)
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}