package main

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	numRecords := len(ids)
	recordSize := 12
	totalSize := numRecords * recordSize
	out := make([]byte, totalSize)
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
			// ID (4 bytes)
			b0 := byte(id)
			out[pos] = b0
			currentHash ^= uint64(b0)
			currentHash *= 1099511628211

			b1 := byte(id >> 8)
			out[pos+1] = b1
			currentHash ^= uint64(b1)
			currentHash *= 1099511628211

			b2 := byte(id >> 16)
			out[pos+2] = b2
			currentHash ^= uint64(b2)
			currentHash *= 1099511628211

			b3 := byte(id >> 24)
			out[pos+3] = b3
			currentHash ^= uint64(b3)
			currentHash *= 1099511628211

			// TS (4 bytes)
			b4 := byte(t)
			out[pos+4] = b4
			currentHash ^= uint64(b4)
			currentHash *= 1099511628211

			b5 := byte(t >> 8)
			out[pos+5] = b5
			currentHash ^= uint64(b5)
			currentHash *= 1099511628211

			b6 := byte(t >> 16)
			out[pos+6] = b6
			currentHash ^= uint64(b6)
			currentHash *= 1099511628211

			b7 := byte(t >> 24)
			out[pos+7] = b7
			currentHash ^= uint64(b7)
			currentHash *= 1099511628211

			// Levels (2 bytes)
			b8 := byte(lv)
			out[pos+8] = b8
			currentHash ^= uint64(b8)
			currentHash *= 1099511628211

			b9 := byte(lv >> 8)
			out[pos+9] = b9
			currentHash ^= uint64(b9)
			currentHash *= 1099511628211

			// Lens (2 bytes)
			b10 := byte(ln)
			out[pos+10] = b10
			currentHash ^= uint64(b10)
			currentHash *= 1099511628211

			b11 := byte(ln >> 8)
			out[pos+11] = b11
			currentHash ^= uint64(b11)
			currentHash *= 1099511628211

			pos += recordSize
		}
		hash = currentHash
	}
	return hash
}