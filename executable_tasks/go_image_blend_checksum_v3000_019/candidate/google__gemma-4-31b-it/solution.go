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
			// Using the identity: (x + 127) / 255 ≈ (x * 0x01010101) >> 24
			// But for small values, we can use the standard formula or a faster approximation.
			// To maintain exact correctness as per the baseline's (s*a + d*invA + 127)/255:
			
			val0 := (uint32(src[base])*a + uint32(dst[base])*invA + 127) / 255
			val1 := (uint32(src[base+1])*a + uint32(dst[base+1])*invA + 127) / 255
			val2 := (uint32(src[base+2])*a + uint32(dst[base+2])*invA + 127) / 255
			
			out[base] = uint8(val0)
			out[base+1] = uint8(val1)
			out[base+2] = uint8(val2)
			out[base+3] = 255
		}
		
		// Inline checksum
		var h uint64 = 1469598103934665603
		for j := 0; j < len(out); j++ {
			h ^= uint64(out[j])
			h *= 1099511628211
		}
		hash = h
	}
	return hash
}