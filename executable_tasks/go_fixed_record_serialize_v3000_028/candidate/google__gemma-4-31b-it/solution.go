package main

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	numRecords := len(ids)
	// Each record is 4 + 4 + 2 + 2 = 12 bytes
	buf := make([]byte, numRecords*12)
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		hash = 1469598103934665603
		pos := 0
		for i := 0; i < numRecords; i++ {
			id := ids[i]
			t := ts[i]
			lvl := levels[i]
			ln := lens[i]

			// Serialize and update hash inline to avoid second pass
			// Record layout: id(4), ts(4), level(2), len(2)
			
			// id
			val := byte(id)
			buf[pos] = val
			hash ^= uint64(val)
			hash *= 1099511628211
			val = byte(id >> 8)
			buf[pos+1] = val
			hash ^= uint64(val)
			hash *= 1099511628211
			val = byte(id >> 16)
			buf[pos+2] = val
			hash ^= uint64(val)
			hash *= 1099511628211
			val = byte(id >> 24)
			buf[pos+3] = val
			hash ^= uint64(val)
			hash *= 1099511628211

			// ts
			val = byte(t)
			buf[pos+4] = val
			hash ^= uint64(val)
			hash *= 1099511628211
			val = byte(t >> 8)
			buf[pos+5] = val
			hash ^= uint64(val)
			hash *= 1099511628211
			val = byte(t >> 16)
			buf[pos+6] = val
			hash ^= uint64(val)
			hash *= 1099511628211
			val = byte(t >> 24)
			buf[pos+7] = val
			hash ^= uint64(val)
			hash *= 1099511628211

			// level
			val = byte(lvl)
			buf[pos+8] = val
			hash ^= uint64(val)
			hash *= 1099511628211
			val = byte(lvl >> 8)
			buf[pos+9] = val
			hash ^= uint64(val)
			hash *= 1099511628211

			// len
			val = byte(ln)
			buf[pos+10] = val
			hash ^= uint64(val)
			hash *= 1099511628211
			val = byte(ln >> 8)
			buf[pos+11] = val
			hash ^= uint64(val)
			hash *= 1099511628211

			pos += 12
		}
	}
	return hash
}