package main

func solutionChecksumBytes(data []byte) uint64 {
	var hash uint64 = 1469598103934665603
	for _, b := range data {
		hash ^= uint64(b)
		hash *= 1099511628211
	}
	return hash
}

func solutionPutU32(dst []byte, v uint32) {
	dst[0] = byte(v)
	dst[1] = byte(v >> 8)
	dst[2] = byte(v >> 16)
	dst[3] = byte(v >> 24)
}

func solutionPutU16(dst []byte, v uint16) {
	dst[0] = byte(v)
	dst[1] = byte(v >> 8)
}

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	out := make([]byte, len(ids)*12)
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		ptr := 0
		for i := range ids {
			solutionPutU32(out[ptr:], ids[i]); ptr += 4
			solutionPutU32(out[ptr:], ts[i]); ptr += 4
			solutionPutU16(out[ptr:], levels[i]); ptr += 2
			solutionPutU16(out[ptr:], lens[i]); ptr += 2
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}
