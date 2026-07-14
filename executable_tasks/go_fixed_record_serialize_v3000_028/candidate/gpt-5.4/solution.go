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
	const recSize = 12
	total := n * recSize

	out := make([]byte, total)
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

			lv := levels[i]
			out[j+8] = byte(lv)
			out[j+9] = byte(lv >> 8)

			ln := lens[i]
			out[j+10] = byte(ln)
			out[j+11] = byte(ln >> 8)

			j += recSize
		}
		hash = solutionChecksumBytes(out)
	}

	return hash
}
