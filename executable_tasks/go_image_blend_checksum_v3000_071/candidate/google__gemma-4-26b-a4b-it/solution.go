package main

func solutionBlendByte(s, d, a uint8) uint8 {
	// Using the fast approximation (s*a + d*(255-a))/255
	// A common high-performance approximation for (x/255) is (x + 1 + (x >> 8)) >> 8
	// However, to ensure exact correctness with the original formula:
	// (s*a + d*(255-a) + 127) / 255
	// We use the integer division property: (val + 127) / 255 ≈ (val + 127 + (val+127)>>8) >> 8
	// But for strict correctness to the provided baseline, we must match the exact math.
	// The baseline uses: uint8((uint32(s)*uint32(a) + uint32(d)*uint32(255-a) + 127) / 255)
	// To optimize, we avoid the division by using the property that x/255 ≈ (x + 1 + (x >> 8)) >> 8 is not quite right for 255.
	// Actually, the most efficient way to do exact division by 255 is: (x + 1 + (x >> 8)) >> 8 is for 256.
	// For 255: (x + 127 + (x + 127) >> 8) >> 8 is also not quite it.
	// Let's use the standard fast way for exact division by 255: (x + 1 + (x >> 8)) >> 8 is for 256.
	// Let's stick to the math but optimize the loop structure.
	return uint8((uint32(s)*uint32(a) + uint32(d)*(255-uint32(a)) + 127) / 255)
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
	pixels := width * height
	out := make([]byte, pixels*4)
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		for i := 0; i < pixels; i++ {
			base := i << 2
			s := src[base]
			d := dst[base]
			a := src[base+3]
			
			// Inline the blending to avoid function call overhead and allow compiler optimizations
			a32 := uint32(a)
			invA := 255 - a32

			// R
			out[base] = uint8((uint32(src[base])*a32 + uint32(dst[base])*invA + 127) / 255)
			// G
			out[base+1] = uint8((uint32(src[base+1])*a32 + uint32(dst[base+1])*invA + 127) / 255)
			// B
			out[base+2] = uint8((uint32(src[base+2])*a32 + uint32(dst[base+2])*invA + 127) / 255)
			// A
			out[base+3] = 255
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}
