package main

import "unsafe"

func run(src, dst []byte, width, height, iters int) uint64 {
	pixels := width * height
	outLen := pixels * 4
	out := make([]byte, outLen)
	
	var hash uint64
	
	for iter := 0; iter < iters; iter++ {
		// Process all pixels with inlined blend
		for i := 0; i < pixels; i++ {
			base := i << 2 // i * 4
			a := uint32(src[base+3])
			invA := 255 - a
			
			// Inlined blend: (s*a + d*invA + 127) / 255
			out[base] = uint8((uint32(src[base])*a + uint32(dst[base])*invA + 127) / 255)
			out[base+1] = uint8((uint32(src[base+1])*a + uint32(dst[base+1])*invA + 127) / 255)
			out[base+2] = uint8((uint32(src[base+2])*a + uint32(dst[base+2])*invA + 127) / 255)
			out[base+3] = 255
		}
		
		// Inlined FNV-1a hash
		hash = 1469598103934665603
		
		// Process 8 bytes at a time
		ptr := unsafe.Pointer(&out[0])
		fullBlocks := outLen >> 3 // outLen / 8
		
		for j := 0; j < fullBlocks; j++ {
			block := *(*uint64)(unsafe.Pointer(uintptr(ptr) + uintptr(j<<3)))
			
			hash ^= uint64(byte(block))
			hash *= 1099511628211
			hash ^= uint64(byte(block >> 8))
			hash *= 1099511628211
			hash ^= uint64(byte(block >> 16))
			hash *= 1099511628211
			hash ^= uint64(byte(block >> 24))
			hash *= 1099511628211
			hash ^= uint64(byte(block >> 32))
			hash *= 1099511628211
			hash ^= uint64(byte(block >> 40))
			hash *= 1099511628211
			hash ^= uint64(byte(block >> 48))
			hash *= 1099511628211
			hash ^= uint64(byte(block >> 56))
			hash *= 1099511628211
		}
		
		// Handle remaining bytes
		for j := fullBlocks << 3; j < outLen; j++ {
			hash ^= uint64(out[j])
			hash *= 1099511628211
		}
	}
	
	return hash
}