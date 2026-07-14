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
	pixels := width * height
	totalLen := len(src)
	out := make([]byte, totalLen)
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		for i := 0; i < pixels; i++ {
			base := i * 4
			srcA := src[base+3]
			a := uint32(srcA)
			ia := uint32(255 - srcA)
			
			// Blend R channel
			srcR := uint32(src[base])
			dstR := uint32(dst[base])
			outR := (srcR*a + dstR*ia + 127) * 0x8081 >> 31
			
			// Blend G channel
			srcG := uint32(src[base+1])
			dstG := uint32(dst[base+1])
			outG := (srcG*a + dstG*ia + 127) * 0x8081 >> 31
			
			// Blend B channel
			srcB := uint32(src[base+2])
			dstB := uint32(dst[base+2])
			outB := (srcB*a + dstB*ia + 127) * 0x8081 >> 31
			
			out[base] = uint8(outR)
			out[base+1] = uint8(outG)
			out[base+2] = uint8(outB)
			out[base+3] = 255
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}