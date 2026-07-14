package main

func solutionChecksumBytes(data []byte) uint64 {
	var hash uint64 = 1469598103934665603
	for _, b := range data {
		hash ^= uint64(b)
		hash *= 1099511628211
	}
	return hash
}

func run(src, dst []byte, width, height, iters int) uint64 {
	out := make([]byte, len(src))
	pixels := width * height
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		for i := 0; i < pixels; i++ {
			base := i * 4
			a := uint32(src[base+3])
			oneMinusA := 255 - a
			
			// Blend red channel
			numR := uint32(src[base])*a + uint32(dst[base])*oneMinusA + 127
			out[base] = uint8((numR * 257) >> 16)
			
			// Blend green channel
			numG := uint32(src[base+1])*a + uint32(dst[base+1])*oneMinusA + 127
			out[base+1] = uint8((numG * 257) >> 16)
			
			// Blend blue channel
			numB := uint32(src[base+2])*a + uint32(dst[base+2])*oneMinusA + 127
			out[base+2] = uint8((numB * 257) >> 16)
			
			// Set output alpha to 255
			out[base+3] = 255
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}