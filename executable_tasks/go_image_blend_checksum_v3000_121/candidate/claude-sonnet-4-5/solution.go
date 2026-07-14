package main

func run(src, dst []byte, width, height, iters int) uint64 {
	pixels := width * height
	out := make([]byte, pixels*4)
	var hash uint64
	
	for iter := 0; iter < iters; iter++ {
		hash = 1469598103934665603
		oi := 0
		
		// Unroll by 4 pixels
		pixels4 := (pixels / 4) * 4
		for i := 0; i < pixels4; i += 4 {
			base := i * 4
			
			// Pixel 0
			a0 := uint32(src[base+3])
			na0 := 255 - a0
			out[oi] = uint8((uint32(src[base])*a0 + uint32(dst[base])*na0 + 127) / 255)
			hash ^= uint64(out[oi])
			hash *= 1099511628211
			oi++
			out[oi] = uint8((uint32(src[base+1])*a0 + uint32(dst[base+1])*na0 + 127) / 255)
			hash ^= uint64(out[oi])
			hash *= 1099511628211
			oi++
			out[oi] = uint8((uint32(src[base+2])*a0 + uint32(dst[base+2])*na0 + 127) / 255)
			hash ^= uint64(out[oi])
			hash *= 1099511628211
			oi++
			out[oi] = 255
			hash ^= 255
			hash *= 1099511628211
			oi++
			
			// Pixel 1
			base += 4
			a1 := uint32(src[base+3])
			na1 := 255 - a1
			out[oi] = uint8((uint32(src[base])*a1 + uint32(dst[base])*na1 + 127) / 255)
			hash ^= uint64(out[oi])
			hash *= 1099511628211
			oi++
			out[oi] = uint8((uint32(src[base+1])*a1 + uint32(dst[base+1])*na1 + 127) / 255)
			hash ^= uint64(out[oi])
			hash *= 1099511628211
			oi++
			out[oi] = uint8((uint32(src[base+2])*a1 + uint32(dst[base+2])*na1 + 127) / 255)
			hash ^= uint64(out[oi])
			hash *= 1099511628211
			oi++
			out[oi] = 255
			hash ^= 255
			hash *= 1099511628211
			oi++
			
			// Pixel 2
			base += 4
			a2 := uint32(src[base+3])
			na2 := 255 - a2
			out[oi] = uint8((uint32(src[base])*a2 + uint32(dst[base])*na2 + 127) / 255)
			hash ^= uint64(out[oi])
			hash *= 1099511628211
			oi++
			out[oi] = uint8((uint32(src[base+1])*a2 + uint32(dst[base+1])*na2 + 127) / 255)
			hash ^= uint64(out[oi])
			hash *= 1099511628211
			oi++
			out[oi] = uint8((uint32(src[base+2])*a2 + uint32(dst[base+2])*na2 + 127) / 255)
			hash ^= uint64(out[oi])
			hash *= 1099511628211
			oi++
			out[oi] = 255
			hash ^= 255
			hash *= 1099511628211
			oi++
			
			// Pixel 3
			base += 4
			a3 := uint32(src[base+3])
			na3 := 255 - a3
			out[oi] = uint8((uint32(src[base])*a3 + uint32(dst[base])*na3 + 127) / 255)
			hash ^= uint64(out[oi])
			hash *= 1099511628211
			oi++
			out[oi] = uint8((uint32(src[base+1])*a3 + uint32(dst[base+1])*na3 + 127) / 255)
			hash ^= uint64(out[oi])
			hash *= 1099511628211
			oi++
			out[oi] = uint8((uint32(src[base+2])*a3 + uint32(dst[base+2])*na3 + 127) / 255)
			hash ^= uint64(out[oi])
			hash *= 1099511628211
			oi++
			out[oi] = 255
			hash ^= 255
			hash *= 1099511628211
			oi++
		}
		
		// Handle remaining pixels
		for i := pixels4; i < pixels; i++ {
			base := i * 4
			a := uint32(src[base+3])
			na := 255 - a
			
			out[oi] = uint8((uint32(src[base])*a + uint32(dst[base])*na + 127) / 255)
			hash ^= uint64(out[oi])
			hash *= 1099511628211
			oi++
			out[oi] = uint8((uint32(src[base+1])*a + uint32(dst[base+1])*na + 127) / 255)
			hash ^= uint64(out[oi])
			hash *= 1099511628211
			oi++
			out[oi] = uint8((uint32(src[base+2])*a + uint32(dst[base+2])*na + 127) / 255)
			hash ^= uint64(out[oi])
			hash *= 1099511628211
			oi++
			out[oi] = 255
			hash ^= 255
			hash *= 1099511628211
			oi++
		}
	}
	
	return hash
}
