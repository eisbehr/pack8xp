# pack8xp
Packs Z80 machine code into the program format for the TI-83+

## Build instructions
The repo includes the simple build.sh script I use for most projects, but there really isn't anything special to building this program.
Just feed it to your compiler without any additional options and it should just compile. Add an -O2 if you want, but it likely makes no real difference.

## Usage
```
Usage: ./pack8xp [input file] [program name]

The input file must be raw Z80 machine code without any headers, checksums, etc.
```
## License
This is in the public domain as far as I am concerned. Do whatever you want with it. Only contribute if you accept that your code will also be treated as public domain.
