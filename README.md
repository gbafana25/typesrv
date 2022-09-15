# typesrv

Server component for multiplayer typetester

## Program flow

- Server listens for connecting players (via poll)
	- new players are added to a file descriptor array
	- poll loop works (sort of) now
- server tells all players when game starts
- clients send current character typed


- note: integer overflow/error occurs on one player when parray size is two, doesn't happen w/ three.

## TODO
- display right/wrong words
- allow changing of player number (config.h)
