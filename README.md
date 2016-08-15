# mhash

mhash (My Hash) aims to be a fast, portable, good general-purpose hashing function.
It essentially use a multiplicative hashing function for small data (less than 8B) and use another hash function for bigger data.

By default it use Intel CRC32C instructions if SSE4.2 is enabled (default) for bigger data, otherwise it defaults to MurmurHash3.

It is very fast for small datas (pointers, IP addresses...) with less than 1 cycle/byte on x86, and CRC32 is also very efficient for bigger data (1 - 2 cycles / byte).

## License

Copyright © 2016 Benoît Ganne

Distributed under the WTFPL v2.

MurmurHash3 was written by Austin Appleby, and is placed in the public
domain.
PMurHash was written by Shane Day, and is also public domain.
