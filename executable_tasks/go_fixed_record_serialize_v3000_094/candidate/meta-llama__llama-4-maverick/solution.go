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
	recordSize := 4 + 4 + 2 + 2
	outputSize := len(ids) * recordSize
	buf := make([]byte, outputSize)
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		for i, id := range ids {
			offset := i * recordSize
			buf[offset] = byte(id)
			buf[offset+1] = byte(id >> 8)
			buf[offset+2] = byte(id >> 16)
			buf[offset+3] = byte(id >> 24)
			ts := ts[i]
			buf[offset+4] = byte(ts)
			buf[offset+5] = byte(ts >> 8)
			buf[offset+6] = byte(ts >> 16)
			buf[offset+7] = byte(ts >> 24)
			level := levels[i]
			len := lens[i]
			buf[offset+8] = byte(level)
			buf[offset+9] = byte(level >> 8)
			buf[offset+10] = byte(len)
			buf[offset+11] = byte(len >> 8)
		}
		hash = solutionChecksumBytes(buf)
	}
	return hash
}
