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
	if n == 0 || iters <= 0 {
		return 1469598103934665603
	}

	const recSize = 12
	out := make([]byte, n*recSize)
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		j := 0
		for i := 0; i < n; i++ {
			v := ids[i]
			out[j+0] = byte(v)
			out[j+1] = byte(v >> 8)
			out[j+2] = byte(v >> 16)
			out[j+3] = byte(v >> 24)

			v = ts[i]
			out[j+4] = byte(v)
			out[j+5] = byte(v >> 8)
			out[j+6] = byte(v >> 16)
			out[j+7] = byte(v >> 24)

			w := levels[i]
			out[j+8] = byte(w)
			out[j+9] = byte(w >> 8)

			w = lens[i]
			out[j+10] = byte(w)
			out[j+11] = byte(w >> 8)

			j += recSize
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}
