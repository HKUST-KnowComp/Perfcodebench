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
	if n == 0 {
		out := make([]byte, 0)
		var hash uint64
		for iter := 0; iter < iters; iter++ {
			hash = solutionChecksumBytes(out)
		}
		return hash
	}

	// Preallocate the exact buffer size
	out := make([]byte, n*12)
	for i := 0; i < n; i++ {
		offset := i * 12
		// id (uint32) little-endian
		v := ids[i]
		out[offset+0] = byte(v)
		out[offset+1] = byte(v >> 8)
		out[offset+2] = byte(v >> 16)
		out[offset+3] = byte(v >> 24)

		// ts (uint32) little-endian
		v = ts[i]
		out[offset+4] = byte(v)
		out[offset+5] = byte(v >> 8)
		out[offset+6] = byte(v >> 16)
		out[offset+7] = byte(v >> 24)

		// levels (uint16) little-endian
		w := levels[i]
		out[offset+8] = byte(w)
		out[offset+9] = byte(w >> 8)

		// lens (uint16) little-endian
		w = lens[i]
		out[offset+10] = byte(w)
		out[offset+11] = byte(w >> 8)
	}

	// Hash the buffer repeatedly
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		hash = solutionChecksumBytes(out)
	}
	return hash
}
