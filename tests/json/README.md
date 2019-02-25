# JSON test application

## About

This application contains an extensive test suite for RIOT builtin JSON parser.
All test cases are derived from the [JSONTestSuite](https://github.com/nst/JSONTestSuite).

## Update Parsing Tests

Parts of the source code are generated with the Python helper script `generate_parsing_test.py`.
To update the test cases with latest revision of the test suite from GitHub run
the following from the `RIOTBASE` folder:

```
cd tests/json
# make backup
mv generated-parsing-tests.c generated-parsing-tests.c.bak
git clone https://github.com/nst/JSONTestSuite
python3 generate_parsing_test.py
```

Afterward (re)build, flash, and run test application again.
