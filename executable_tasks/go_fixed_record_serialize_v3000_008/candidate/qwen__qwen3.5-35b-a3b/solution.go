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
	var out []byte
	var hash uint64
	n := len(ids)
	size := n * 12
	out = make([]byte, size)
	for iter := 0; iter < iters; iter++ {
		for i := 0; i < n; i++ {
			offset := i * 12
			out[offset] = byte(ids[i])
			out[offset+1] = byte(ids[i] >> 8)
			out[offset+2] = byte(ids[i] >> 16)
			out[offset+3] = byte(ids[i] >> 24)

			out[offset+4] = byte(ts[i])
			out[offset+5] = byte(ts[i] >> 8)
			out[offset+6] = byte(ts[i] >> 16)
			out[offset+7] = byte(ts[i] >> 24)

			out[offset+8] = byte(levels[i])
			out[offset+9] = byte(levels[i] >> 8)
			out[offset+10] = byte(lens[i])
			out[offset+11] = byte(lens[i] >> 8)
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}