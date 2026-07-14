package main

import "unsafe"

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	n := len(ids)
	recordSize := 12 // 4 + 4 + 2 + 2 bytes per record
	totalSize := n * recordSize
	out := make([]byte, totalSize)

	var hash uint64
	for iter := 0; iter < iters; iter++ {
		// Serialize all records using unsafe pointer writes
		off := 0
		for i := 0; i < n; i++ {
			// Write id (4 bytes) - little endian
			*(*uint32)(unsafe.Pointer(&out[off])) = ids[i]
			off += 4
			// Write ts (4 bytes) - little endian
			*(*uint32)(unsafe.Pointer(&out[off])) = ts[i]
			off += 4
			// Write levels and lens (2 bytes each) - little endian
			*(*uint16)(unsafe.Pointer(&out[off])) = levels[i]
			off += 2
			*(*uint16)(unsafe.Pointer(&out[off])) = lens[i]
			off += 2
		}

		// FNV-1a hash with unrolled loop
		hash = 1469598103934665603
		ptr := unsafe.Pointer(&out[0])
		
		// Process 8 bytes at a time
		fullChunks := totalSize >> 3
		for j := 0; j < fullChunks; j++ {
			b := *(*[8]byte)(ptr)
			hash ^= uint64(b[0])
			hash *= 1099511628211
			hash ^= uint64(b[1])
			hash *= 1099511628211
			hash ^= uint64(b[2])
			hash *= 1099511628211
			hash ^= uint64(b[3])
			hash *= 1099511628211
			hash ^= uint64(b[4])
			hash *= 1099511628211
			hash ^= uint64(b[5])
			hash *= 1099511628211
			hash ^= uint64(b[6])
			hash *= 1099511628211
			hash ^= uint64(b[7])
			hash *= 1099511628211
			ptr = unsafe.Pointer(uintptr(ptr) + 8)
		}
		
		// Handle remaining bytes
		remaining := totalSize & 7
		if remaining > 0 {
			base := fullChunks << 3
			for j := 0; j < remaining; j++ {
				hash ^= uint64(out[base+j])
				hash *= 1099511628211
			}
		}
	}
	return hash
}