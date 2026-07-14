package main

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	numRecords := len(ids)
	// Each record is 4 + 4 + 2 + 2 = 12 bytes
	buf := make([]byte, numRecords*12)
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		for i := 0; i < numRecords; i++ {
			pos := i * 12
			id := ids[i]
			t := ts[i]
			lvl := levels[i]
			ln := lens[i]

			buf[pos] = byte(id)
			buf[pos+1] = byte(id >> 8)
			buf[pos+2] = byte(id >> 16)
			buf[pos+3] = byte(id >> 24)
			buf[pos+4] = byte(t)
			buf[pos+5] = byte(t >> 8)
			buf[pos+6] = byte(t >> 16)
			buf[pos+7] = byte(t >> 24)
			buf[pos+8] = byte(lvl)
			buf[pos+9] = byte(lvl >> 8)
			buf[pos+10] = byte(ln)
			buf[pos+11] = byte(ln >> 8)
		}

		// FNV-1a style checksum
		hash = 1469598103934665603
		for j := 0; j < len(buf); j++ {
			hash ^= uint64(buf[j])
			hash *= 1099511628211
		}
	}
	return hash
}