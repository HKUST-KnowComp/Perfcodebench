package main

func solutionBlendByte(s, d, a uint8) uint8 {
	return uint8((uint32(s)*uint32(a) + uint32(d)*(255 - a) + 127) / 255)
}

func solutionChecksumBytes(data []byte) uint64 {
	var hash uint64 = 1469598103934665603
	for i := 0; i < len(data); i += 4 {
		hash ^= uint64(data[i])
		hash *= 1099511628211
		hash ^= uint64(data[i+1])
		hash *= 1099511628211
		hash ^= uint64(data[i+2])
		hash *= 1099511628211
		hash ^= uint64(data[i+3])
		hash *= 1099511628211
	}
	return hash
}

func run(src, dst []byte, width, height, iters int) uint64 {
	pixels := width * height
	out := make([]byte, 4*pixels)
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		for i := 0; i < pixels; i++ {
			base := i * 4
			a := src[base+3]
			out[base] = solutionBlendByte(src[base], dst[base], a)
			out[base+1] = solutionBlendByte(src[base+1], dst[base+1], a)
			out[base+2] = solutionBlendByte(src[base+2], dst[base+2], a)
			out[base+3] = 255
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}