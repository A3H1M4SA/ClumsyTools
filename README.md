# ClumsyTools - PS3 Homebrew Application

A starter template for PS3 homebrew development using PSL1GHT SDK.

## Project Structure

```
ClumsyTools/
├── Makefile              # Build configuration
├── sfo.xml               # Application metadata
├── source/               # Source code
│   ├── main.c           # Main application entry point
│   ├── rsxutil.c        # RSX graphics utilities
│   └── rsxutil.h        # RSX headers
├── include/             # Additional headers (if needed)
├── pkgfiles/            # Files to include in PKG
│   ├── ICON0.PNG        # App icon (320x176)
│   └── USRDIR/          # Application data directory
└── build/               # Build output (generated)
```

## Application Info

- **Title:** ClumsyTools App
- **App ID:** NP00CLMSY
- **Content ID:** UP0001-NP00CLMSY_00-0000000000000000
- **Version:** 1.0.0

## Building

### Prerequisites

1. PSL1GHT SDK installed and configured
2. PS3 toolchain in PATH
3. Required environment variables:
   - `PSL1GHT` - Path to PSL1GHT installation
   - `PS3DEV` - Path to PS3 development tools

### Build Commands

```bash
# Full build (compile + create PKG)
make pkg

# Clean build files
make clean

# Build and run on PS3 (requires ps3load)
make run
```

### Output Files

- `clumsytools.elf` - Unencrypted executable
- `clumsytools.self` - Signed executable
- `EBOOT.BIN` - Encrypted executable for PKG
- `clumsytools.pkg` - Base package
- `clumsytools.gnpdrm.pkg` - **Final signed PKG** (install this!)

## Installation

1. Copy `clumsytools.gnpdrm.pkg` to your PS3 (via USB or FTP)
2. Install using Package Manager
3. Launch from XMB under "Game" category

## Customization

### Change App Name

Edit `Makefile`:
```makefile
TITLE := Your App Name
```

Edit `sfo.xml`:
```xml
<value name="TITLE" type="string">
    Your App Name
</value>
```

### Change App ID

Edit `Makefile`:
```makefile
APPID := NP00XXXXX
```

Edit `sfo.xml`:
```xml
<value name="TITLE_ID" type="string">
    NP00XXXXX
</value>
```

### Add Icon

Create/add a 320x176 PNG file at:
```
pkgfiles/ICON0.PNG
```

### Add Additional Files

Place any files you want included in the PKG in:
```
pkgfiles/USRDIR/
```

## Development

The starter code displays colored rectangles and a simple animation. Modify `source/main.c` to build your application.

### Useful Libraries

The Makefile is configured to link:
- `-lrt` - Real-time library
- `-llv2` - LV2 system calls
- `-lsysutil` - System utilities
- `-lrsx` - RSX graphics
- `-lnet` - Networking
- `-lio` - I/O (pad, mouse, kb)

## License

This is a homebrew development template. Modify and use as needed for your projects.

## Resources

- [PSL1GHT Documentation](https://github.com/ps3dev/PSL1GHT)
- [PS3 Homebrew Development](https://www.ps3devwiki.com/)

