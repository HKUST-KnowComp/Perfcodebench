package main

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	numRecords := len(ids)
	recordSize := 12
	bufSize := numRecords * recordSize
	out := make([]byte, bufSize)
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		var currentHash uint64 = 1469598103934665603
		pos := 0
		for i := 0; i < numRecords; i++ {
			id := ids[i]
			t := ts[i]
			lv := levels[i]
			ln := lens[i]

			// Serialize and hash in one pass
			val := byte(id)
			out[pos] = val
			currentHash ^= uint64(val)
			currentHash *= 1099511628211
			pos++

			val = byte(id >> 8)
			out[pos] = val
			currentHash ^= uint64(val)
			currentHash *= 1099511628211
			pos++

			val = byte(id >> 16)
			out[pos] = val
			currentHash ^= uint64(val)
			currentHash *= 1099511628211
			pos++

			val = byte(id >> 24)
			out[pos] = val
			currentHash ^= uint64(val)
			currentHash *= 1099511628211
			pos++

			val = byte(t)
			out[pos] = val
			currentHash ^= uint64(val)
			currentHash *= 1099511628211
			pos++

			val = byte(t >> 8)
			out[pos] = val
			currentHash ^= uint64(val)
			currentHash *= 1099511628211
			pos++

			val = byte(t >> 16)
			out[pos] = val
			currentHash ^= uint64(val)
			currentHash *= 1099511628211
			pos++

			val = byte(t >> 24)
			out[pos] = val
			currentHash ^= uint64(val)
			currentHash *= 1099511628211
			pos++

			val = byte(lv)
			out[pos] = val
			currentHash ^= uint64(val)
			currentHash *= 1099511628211
			pos++

			val = byte(lv >> 8)
			out[pos] = val
			currentHash ^= uint64(val)
			currentHash *= 1099511628211
			pos++

			val = byte(ln)
			out[pos] = val
			currentHash ^= uint64(val)
			currentHash *= 1099511628211
			pos++

			val = byte(ln >> 8)
			out[pos] = val
			currentHash ^= uint64(val)
			currentHash *= 1099511628211
			pos++
		}
		hash = currentHash
	}
	return hash
}