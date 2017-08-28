# Collection of Removed Features and Modules

This file contains a listing of all features and modules that were removed from
RIOT at some point in time, mostly due to missing maintenance. The purpose of
this list is to have a reference point for reintegrating those features in the
future, if new interest arises or a new maintainer is found.

This list is **not** supposed to contain a change log of all the things that are
being removed from RIOT, but it should contain only full modules that are
removed without any alternative being merged at the same time. For example
things like the first `netdev` should not be put in this list, as a successor
(former `netdev2`) has been merged.


## How to read this file

For each high-level feature removed there should be one entry in this file. The
entries should comply to the following template:
```
### path/feature_name [HASH of removal commit]
Author(s):
- author 1 <foo.bar@abc.com>
- author 2 <a.b@c.net>
Reason for removal:
- give a short and comprehensive argumentation why this feature was removed
- typical reasons are:
- feature is not maintained anymore
- lack of hardware so feature can not be tested (anymore)
- feature was never used and there is no reason for keeping it
```

By putting the name of the removed feature and the commit hash in the same line,
one can very quickly find the commit using `get grep`.

Listing the authors of the removed code is done for appreciation of their work.
This way, there names are never removed from the RIOT repository.


## Removed Features

### boards/weio [8ccf51118d74a585c1ff3fd7922ca8189b61ebbc]
Author(s):
- Paul Rathgeb <paul.rathgeb@skynet.be>

Reason for removal:
- hardware not available to the community for testing
- original author and maintainer won't be able to maintain the code

### cpu/lpc11u34 [5d85808010e72573e949f79b6b010609ca75a205]
Author(s):
- Paul Rathgeb <paul.rathgeb@skynet.be>

Reason for removal:
- hardware not available to the community for testing
- original author and maintainer won't be able to maintain the code
