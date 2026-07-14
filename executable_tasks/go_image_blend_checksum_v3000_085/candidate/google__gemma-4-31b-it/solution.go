package main

func run(src, dst []byte, width, height, iters int) uint64 {
	pixels := width * height
	out := make([]byte, pixels*4)
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		for i := 0; i < pixels; i++ {
			base := i * 4
			a := uint32(src[base+3])
			invA := 255 - a
			
			// Fast blend: (s*a + d*(255-a) + 127) / 255
			// Using the identity: (x + 127) / 255 ≈ (x * 0x8081) >> 23
			// However, for exactness with the provided formula, we use the standard integer math
			// but avoid the function call overhead.
			
			out[base] = uint8((uint32(src[base])*a + uint32(dst[base])*invA + 127) / 255)
			out[base+1] = uint8((uint32(src[base+1])*a + uint32(dst[base+1])*invA + 127) / 255)
			out[base+2] = uint8((uint32(src[base+2])*a + uint32(dst[base+2])*invA + 127) / 255)
			out[base+3] = 255
		}
		
		// Inline checksum
		var h uint64 = 1469598103934665603
		for _, b := range out {
			h ^= uint64(b)
			h *= 1099511628211
		}
		hash = h
	}
	return hash
}