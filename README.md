# typesrv

Server component for multiplayer typetester

## Program flow

- Server listens for connecting players (via poll)
	- new players are added to a file descriptor array
	- poll loop works (sort of) now
- server tells all players when game starts
- clients send current character typed
- server sends back if it's right or wrong
	- clients cursor doesn't move if wrong



## TODO
- allow changing of player number (config.h)
- track player standings (after finishing + during race)
- display place in real time (?)
