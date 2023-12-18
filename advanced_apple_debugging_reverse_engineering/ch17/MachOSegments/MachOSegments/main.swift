//

import Foundation
import MachO

func convertIntTupleToString(name: Any) -> String {
    var returnString = ""
    let mirror = Mirror(reflecting: name)
    //          helper class to iterate Tuple in swift
    for child in mirror.children {
        guard let val = child.value as? Int8, val != 0  else {
            break;
        }
        returnString.append(Character(UnicodeScalar(UInt8(val))))
        //                           ^ smallest unicode unit
    }
    return returnString
}

// list each linked lib
for i in 0..<_dyld_image_count() {
    let imagePath = String(validatingUTF8: _dyld_get_image_name(i))!
    let imageName = (imagePath as NSString).lastPathComponent
    let header = _dyld_get_image_header(i)!
    print("\(i) \(imageName) \(header)")
    
    var curLoadCommandIterator = Int(bitPattern: header) + MemoryLayout<mach_header_64>.size
    // each `header` followed by
    //  mach_header_64
    //  load_command
    for _ in 0..<header.pointee.ncmds { // each load command
        let loadCommand = UnsafePointer<load_command>(bitPattern: curLoadCommandIterator)!.pointee
        //                            ^ reinterprate pointer type
        //                                                                          ^ dereferece reference pointer
        if loadCommand.cmd == LC_SEGMENT_64 { // for load command LC_SEGMENT_64
            let segmentCommand = UnsafePointer<segment_command_64>(bitPattern: curLoadCommandIterator)!.pointee
            // segment_command_64 is load_command's sub type
            let segName = convertIntTupleToString(name: segmentCommand.segname)
            print("\t\(segName)")
            
            for j in 0..<segmentCommand.nsects { // each sub sections
                let sectionOffset = curLoadCommandIterator + MemoryLayout<segment_command_64>.size
                let offset = MemoryLayout<section_64>.size * Int(j)
                let sectionCommand = UnsafePointer<section_64>(bitPattern: sectionOffset + offset)!.pointee
                let sectionName = convertIntTupleToString(name: sectionCommand.sectname)
                print("\t\t\(sectionName)")
            }
        }
        
        curLoadCommandIterator = curLoadCommandIterator + Int(loadCommand.cmdsize)
    }
}


// prevent app from exiting
CFRunLoopRun()
