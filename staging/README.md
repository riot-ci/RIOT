# Introduction

Welcome to the staging tree!
This tree contains experimental code that does not (yet) meet the quality
requirements of RIOT.

**WARNING**

Do not depend on anything within this subfolder!  Everything in here might be
broken or break or be changed or removed at any time, and everything is
completely unsupported! API's from this tree might change without notice.

## How to use

Please re-read the warning above.
If you still feel brave enough to use code contained in here, add

    ENABLE_STAGING=1

to your application's Makefile.

## Terminology

"main tree" is used to describe the regular RIOT sources (everything outside
staging/).

"broken" means, it doesn't compile or work anymore.

## Rules

- no dependencies from main tree into staging are allowed
- commits MUST NOT change files both in and outside of staging/
- if anything in staging/ breaks due to a change in master, it will be marked
  as broken
- the original contributor is considered maintainer for a module
- non-trivial changes SHOULD be ACK'ed by a module's maintainer
- PR's to staging/ are labeled with "staging"
- PR's changing both staging and main tree code are *not* labeled "staging"

## Procedures

### Reviewing a change to staging/

TBD

### Mark as "broken"

Modules can be marked as broken in staging/staging.mk.
Add a comment with a date and a reason, e.g.,

    BROKEN += heart   # 2018/12/24: broken through change in friend API (#12345)

### Advance to main tree

If a module in staging/ has matured enough to be deemed fit for the main tree,
anyone can open a PR that moves the corresponding code out of staging/. At this
point, a full review of the code has to be made.
The PR SHOULD do the move in one commit that only contains the file moves
(after any possibly necessary other commits doing other changes).

### Removal from staging

During the final stages of a release, the release manager creates a PR removing
all modules that have been marked "broken" for more than two release cycles.
