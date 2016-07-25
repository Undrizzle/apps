
            Sample for CRC Checker and Counter
=======================================================

This sample shows how to enable/disable CRC Checker and how to 
read/clear CRC Error counter and Frame counter.

Main Functions:
    enableCRCChecker  - enables CRC Checker so that CRC Counter is valid
    disableCRCChecker - disables CRC Checker
    clearCRCCounter   - clear CRC Error Counter and Frame Counter
    readCRCCounter    - read CRC Error Counter and Frame Counter

Used APIs:
    mdStatsSetCRCCheckerEnable,
    mdStatsGetCRCErrorCounter,
    mdStatsGetFrameCounter
			







