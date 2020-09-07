# LibraryObscura
[Obfuscated C](https://www.ioccc.org/) Web API to mimic a basic library.

This is, apparently, what I write when given a basic challenge, but the point is "having fun with it."

## Running
Install docker and run `make test`.

-- or --

Install docker and run `make start` and refer to test.sh for the various `curl` commands to interact with the library.

-- or --

Run install `libmicrohttpd` and its headers and run `make all && ./library`.

## Warnings
- The obfuscation process resulted in memory leaks.
- Beware of possible buffer overruns and underruns.
- The code is wider than it is tall.
