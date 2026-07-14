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
		pos := 0
		for i := 0; i < n; i++ {
			_ = out[pos+11] // eliminate bounds checks for this iteration

			id := ids[i]
			out[pos] = byte(id)
			out[pos+1] = byte(id >> 8)
			out[pos+2] = byte(id >> 16)
			out[pos+3] = byte(id >> 24)

			t := ts[i]
			out[pos+4] = byte(t)
			out[pos+5] = byte(t >> 8)
			out[pos+6] = byte(t >> 16)
			out[pos+7] = byte(t >> 24)

			lv := levels[i]
			out[pos+8] = byte(lv)
			out[pos+9] = byte(lv >> 8)

			ln := lens[i]
			out[pos+10] = byte(ln)
			out[pos+11] = byte(ln >> 8)

			pos += 12
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}