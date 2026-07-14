package main

func solutionChecksumBytes(data []byte) uint64 {
	var hash uint64 = 1469598103934665603
	for i := 0; i < len(data); i++ {
		hash ^= uint64(data[i])
		hash *= 1099511628211
	}
	return hash
}

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	n := len(ids)
	if n == 0 || iters <= 0 {
		return solutionChecksumBytes(nil)
	}

	const recSize = 12
	out := make([]byte, n*recSize)
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		j := 0
		for i := 0; i < n; i++ {
			v0 := ids[i]
			out[j+0] = byte(v0)
			out[j+1] = byte(v0 >> 8)
			out[j+2] = byte(v0 >> 16)
			out[j+3] = byte(v0 >> 24)

			v1 := ts[i]
			out[j+4] = byte(v1)
			out[j+5] = byte(v1 >> 8)
			out[j+6] = byte(v1 >> 16)
			out[j+7] = byte(v1 >> 24)

			v2 := levels[i]
			out[j+8] = byte(v2)
			out[j+9] = byte(v2 >> 8)

			v3 := lens[i]
			out[j+10] = byte(v3)
			out[j+11] = byte(v3 >> 8)

			j += recSize
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}
