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
		off := 0
		for i := 0; i < n; i++ {
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
			v16 := levels[i]
			out[off+8] = byte(v16)
			out[off+9] = byte(v16 >> 8)
			v16 = lens[i]
			out[off+10] = byte(v16)
			out[off+11] = byte(v16 >> 8)
			off += 12
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}