import AppKit
import UniformTypeIdentifiers

@_cdecl("open_file_dialog")
public func open_file_dialog() -> UnsafeMutablePointer<CChar>? {
    let dialog = NSOpenPanel()

    dialog.title = "Choose a rom"
    dialog.showsHiddenFiles = false
    dialog.canChooseDirectories = false
    dialog.allowsMultipleSelection = false
    dialog.allowedContentTypes = [
    UTType(filenameExtension: "ch8")!
]

    if dialog.runModal() == .OK {
        if let url = dialog.url {
            return strdup(url.path)
        }
    }
    return nil
}