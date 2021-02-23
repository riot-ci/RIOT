Basic tests for the CongURE API
===============================

This test tests the `congure_test` test framework used for the other CongURE
tests.

Usage
-----

The test requires an up-to-date version of `riotctrl` with `rapidjson` support:

```console
$ pip install --upgrade riotctrl[rapidjson]
```

Then simply run the application using:

```console
$ BOARD="<board>" make flash test
```

It can also executed with pytest:

```console
$ pytest tests/01-run.py
```

Expected result
---------------

The application's test script passes without error code.
