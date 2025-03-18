# ![nc-scout](img/nc-scout.png)

## Index
* [Description](#description)
* [Dependencies](#dependencies)
* [Usage](#usage)
* [Build](#build-instructions)
* [Installation](#installation)

## Description
nc-scout is a simple naming convention checker tool. It is intended to help you enforce naming conventions on directory structures. 

## Dependencies
* make
* POSIX-compliant system (Linux, MacOS)

## Usage
The layout of a nc-scout command.

nc-scout [OPTION]? [COMMAND] [CONVENTION] [DIRECTORY]

### Options
Options are command specific. To learn about the specific options available for a command, use `nc-scout [COMMAND] --help`

### Commands
|Command            | Description                                                                      |
|-------------------|----------------------------------------------------------------------------------|
| `search`          | Search a directory for files that do not match a naming convention.              |
| `analyze`         | Displays the percentage of files that follow a naming convention in a directory. |

### Conventions
| Convention        | Example                                                                          |
|-------------------|----------------------------------------------------------------------------------|
| `camelcase`       | exampleFileName.txt                                                              |
| `cobolcase`       | EXAMPLE-FILE-NAME.txt                                                            |
| `constantcase`    | EXAMPLE_FILE_NAME.txt                                                            |
| `flatcase`        | examplefilename.txt                                                              |
| `kebabcase`       | example-file-name.txt                                                            |
| `pascalcase`      | ExampleFileName.txt                                                              |
| `snakecase`       | example_file_name.txt                                                            |

### What is the Filename Body of a Filename?
The **filename body** is the text of a file's full filename, ignoring leading periods and file extentions. The final period itself and the text that follows it is what is defined as the file extention. `search` is only performed on the filename body of a filename.

```bash
# Search for matches:
nc-scout search snakecase --matches ./
# Output:
.example_file           # The leading period is ignored, resulting in the filename body 'example_file', which is snakecase.
example_file.txt        # The file extention '.txt' is ignored, resulting in the filename body 'example_file', which is snakecase.
.example_file.RAR       # The file extention '.RAR' is ignored, resulting in the filename body 'example_file', which is snakecase.

# Search for non-matches: 
nc-scout search flatcase ./
# Output:
..example_file          # The leading period is ignored, resulting in the filename body '.example', which is not snakecase.
example_file.exe.txt    # The file extention '.txt' is ignored, resulting in the filename body 'example_file.exe', which is not snakecase.

```

### Strict vs. Lenient:
The default enforcement of naming conventions for a search is lenient, although, using
the `-s` or `--strict` option, you can strictly enforce the naming convention for that search.

Strict enforcement means that the naming convention **must** be present in it's entirety in the filename body, while lenient enforcement means that the naming convention **could** be present in it's entirety in the filename body if more text is added, but not removed or changed.

Example File: **example.txt** <em>(filename body is 'example')</em>

Strict matches:
* `flatcase` - The filename body <em>'example'</em> is all lowercase, so it strictly matches the convention.

Lenient matches:
* `kebabcase` - The filename body <em>'example'</em> could be extended to <em>'example-file'</em>, so it leniently matches the convention.
* `snakecase` - the filename body <em>'example'</em> could be extended to <em>'example_file'</em>, so it leniently matches the convention.
* `camelcase` - The filename body <em>'example'</em> could be extended to <em>'exampleFile'</em>, so it leniently matches the convention.

## Build Instructions
To begin, clone the project and go to the root of the repository:
```bash
git clone https://github.com/BlinkDynamo/nc-scout.git nc-scout
cd nc-scout/
```

Build the binary:
```bash
make
``` 

Run tests (optional):
```bash
make check
```

## Installation
If the project built successfully, you can now either use the binary directly from the repository, or install it to your system.

To use the binary directly:
```bash
cd build/
# called directly from the build directory.
./nc-scout --help
```

To install the binary and use it systemwide:
```bash
sudo make install
# now installed in /usr/local/bin/
nc-scout --help
```

Should you want to clean the build/ and tests/ directories:
```bash
make clean
```

To uninstall the binary:
```bash
sudo make uninstall
# rm -f /usr/local/bin/nc-scout
```
