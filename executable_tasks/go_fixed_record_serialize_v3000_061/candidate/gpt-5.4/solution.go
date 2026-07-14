package main

func solutionChecksumBytes(data []byte) uint64 {
	var hash uint64 = 1469598103934665603
	for i := 0; i < len(data); i++ {
		hash ^= uint64(data[i])
		hash *= 1099511628211
	}
	return hash
}

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	n := len(ids)
	total := n * 12
	out := make([]byte, total)
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		p := 0
		for i := 0; i < n; i++ {
			v := ids[i]
			out[p+0] = byte(v)
			out[p+1] = byte(v >> 8)
			out[p+2] = byte(v >> 16)
			out[p+3] = byte(v >> 24)

			v = ts[i]
			out[p+4] = byte(v)
			out[p+5] = byte(v >> 8)
			out[p+6] = byte(v >> 16)
			out[p+7] = byte(v >> 24)

			w := levels[i]
			out[p+8] = byte(w)
			out[p+9] = byte(w >> 8)

			w = lens[i]
			out[p+10] = byte(w)
			out[p+11] = byte(w >> 8)

			p += 12
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}
