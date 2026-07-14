package main

func solutionChecksumBytes(data []byte) uint64 {
	var hash uint64 = 1469598103934665603
	for i := 0; i < len(data); i++ {
		hash ^= uint64(data[i])
		hash *= 1099511628211
	}
	return hash
}

func run(src, dst []byte, width, height, iters int) uint64 {
	pixels := width * height
	n := pixels * 4
	out := make([]byte, n)

	// Precompute exact rounded division by 255 for all possible
	// x = s*a + d*(255-a), where x is in [0, 65025].
	var div255 [65026]uint8
	for i := 0; i <= 65025; i++ {
		div255[i] = uint8((i + 127) / 255)
	}

	var hash uint64
	for iter := 0; iter < iters; iter++ {
		for si, oi := 0, 0; oi < n; si, oi = si+4, oi+4 {
			a := int(src[si+3])
			ia := 255 - a

			s0 := int(src[si])
			d0 := int(dst[si])
			out[oi] = div255[s0*a+d0*ia]

			s1 := int(src[si+1])
			d1 := int(dst[si+1])
			out[oi+1] = div255[s1*a+d1*ia]

			s2 := int(src[si+2])
			d2 := int(dst[si+2])
			out[oi+2] = div255[s2*a+d2*ia]

			out[oi+3] = 255
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}
