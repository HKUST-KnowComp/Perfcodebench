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
	// Each record is 4 (id) + 4 (ts) + 2 (level) + 2 (len) = 12 bytes
	recordSize := 12
	bufSize := n * recordSize
	
	// Pre-allocate the buffer once to avoid reallocations in the loop
	out := make([]byte, bufSize)
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		idx := 0
		for i := 0; i < n; i++ {
			id := ids[i]
			t := ts[i]
			l := levels[i]
			nlen := lens[i]

			// Manual unrolled serialization for speed
			out[idx] = byte(id)
			out[idx+1] = byte(id >> 8)
			out[idx+2] = byte(id >> 16)
			out[idx+3] = byte(id >> 24)
			idx += 4

			out[idx] = byte(t)
			out[idx+1] = byte(t >> 8)
			out[idx+2] = byte(t >> 16)
			out[idx+3] = byte(t >> 24)
			idx += 4

			out[idx] = byte(l)
			out[idx+1] = byte(l >> 8)
			idx += 2

			out[idx] = byte(nlen)
			out[idx+1] = byte(nlen >> 8)
			idx += 2
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}