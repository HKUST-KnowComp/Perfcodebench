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
	out := make([]byte, 0, len(ids)*10*iters)
	buf := make([]byte, 10)
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		out = out[:0]
		for i := range ids {
			buf[0] = byte(ids[i])
			buf[1] = byte(ids[i] >> 8)
			buf[2] = byte(ids[i] >> 16)
			buf[3] = byte(ids[i] >> 24)
			buf[4] = byte(ts[i])
			buf[5] = byte(ts[i] >> 8)
			buf[6] = byte(ts[i] >> 16)
			buf[7] = byte(ts[i] >> 24)
			buf[8] = byte(levels[i])
			buf[9] = byte(levels[i] >> 8)
			out = append(out, buf[:8]...)
			out = append(out, buf[8], byte(lens[i]>>8))
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}