package main

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	n := len(ids)
	buf := make([]byte, 12*n)
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		off := 0
		for i := 0; i < n; i++ {
			id := ids[i]
			buf[off] = byte(id)
			buf[off+1] = byte(id >> 8)
			buf[off+2] = byte(id >> 16)
			buf[off+3] = byte(id >> 24)
			off += 4

			tsVal := ts[i]
			buf[off] = byte(tsVal)
			buf[off+1] = byte(tsVal >> 8)
			buf[off+2] = byte(tsVal >> 16)
			buf[off+3] = byte(tsVal >> 24)
			off += 4

			level := levels[i]
			buf[off] = byte(level)
			buf[off+1] = byte(level >> 8)
			off += 2

			length := lens[i]
			buf[off] = byte(length)
			buf[off+1] = byte(length >> 8)
			off += 2
		}

		hash = 1469598103934665603
		for _, b := range buf {
			hash ^= uint64(b)
			hash *= 1099511628211
		}
	}
	return hash
}
