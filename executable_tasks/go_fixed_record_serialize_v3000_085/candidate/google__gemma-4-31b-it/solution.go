package main

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	numRecords := len(ids)
	// Each record is 4 + 4 + 2 + 2 = 12 bytes
	bufSize := numRecords * 12
	out := make([]byte, bufSize)
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		hash = 1469598103934665603
		for i := 0; i < numRecords; i++ {
			id := ids[i]
			t := ts[i]
			lvl := levels[i]
			ln := lens[i]

			pos := i * 12
			out[pos] = byte(id)
			out[pos+1] = byte(id >> 8)
			out[pos+2] = byte(id >> 16)
			out[pos+3] = byte(id >> 24)
			out[pos+4] = byte(t)
			out[pos+5] = byte(t >> 8)
			out[pos+6] = byte(t >> 16)
			out[pos+7] = byte(t >> 24)
			out[pos+8] = byte(lvl)
			out[pos+9] = byte(lvl >> 8)
			out[pos+10] = byte(ln)
			out[pos+11] = byte(ln >> 8)

			// Inline checksum calculation to avoid second pass
			for j := 0; j < 12; j++ {
				hash ^= uint64(out[pos+j])
				hash *= 1099511628211
			}
		}
	}
	return hash
}