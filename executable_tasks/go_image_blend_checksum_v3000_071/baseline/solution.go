package main

func solutionBlendByte(s, d, a uint8) uint8 {
	return uint8((uint32(s)*uint32(a) + uint32(d)*uint32(255-a) + 127) / 255)
}

func solutionChecksumBytes(data []byte) uint64 {
	var hash uint64 = 1469598103934665603
	for _, b := range data {
		hash ^= uint64(b)
		hash *= 1099511628211
	}
	return hash
}

func run(src, dst []byte, width, height, iters int) uint64 {
	out := make([]byte, 0, len(src))
	var hash uint64
	pixels := width * height
	for iter := 0; iter < iters; iter++ {
		out = out[:0]
		for i := 0; i < pixels; i++ {
			base := i * 4
			a := src[base+3]
			out = append(out,
				solutionBlendByte(src[base], dst[base], a),
				solutionBlendByte(src[base+1], dst[base+1], a),
				solutionBlendByte(src[base+2], dst[base+2], a),
				255,
			)
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}
