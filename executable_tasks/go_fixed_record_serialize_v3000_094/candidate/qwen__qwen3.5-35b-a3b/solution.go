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
	out := make([]byte, 0, n*12)
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		out = out[:0]
		for i := 0; i < n; i++ {
			idx := i * 12
			out[idx] = byte(ids[i])
			out[idx+1] = byte(ids[i] >> 8)
			out[idx+2] = byte(ids[i] >> 16)
			out[idx+3] = byte(ids[i] >> 24)
			out[idx+4] = byte(ts[i])
			out[idx+5] = byte(ts[i] >> 8)
			out[idx+6] = byte(ts[i] >> 16)
			out[idx+7] = byte(ts[i] >> 24)
			out[idx+8] = byte(levels[i])
			out[idx+9] = byte(levels[i] >> 8)
			out[idx+10] = byte(lens[i])
			out[idx+11] = byte(lens[i] >> 8)
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}