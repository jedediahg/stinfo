# stinfo (Safetensors Info)

This program reads the JSON metadata from a `.safetensors` file and outputs it to the console. It supports options for verbose output, pretty-printing the JSON, and printing only the `__metadata__` object.

## Usage

stinfo [options] <filename>

### Options

- `-v`: Verbose output, prints the JSON header size.
- `-p`: Prettify the JSON output with indentation.
- `-m`: Print only the `__metadata__` object from the JSON.
- `--?`: Show this help message and exit.

### Example Usage

- Print the JSON metadata with verbose output:
stinfo -v filename.safetensors

- Pretty-print the JSON metadata:
stinfo -p filename.safetensors

- Print only the `__metadata__` object:
stinfo -m filename.safetensors

- Show help message:
stinfo --?

## Building the Program

To compile the program, you can use GCC:

```sh
gcc stinfo.c -o stinfo
```
## License
This project is licensed under the MIT License.
