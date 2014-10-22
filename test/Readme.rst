==========
Unit Tests
==========

Unit testing a vital part of modern programming practice as it provides a
way to rigorously test a variety of conditions and allows any bugs to be 
squashed more easily during the development cycle. This is particularly 
true for CLEED as the code is the accumulation of many different works 
- all written either by different authors or at different times (when 
ideology or approaches may have changed). The code is also not yet 
stable and is still undergoing frequent changes. In such a scenario 
it is paramount that changes do not lead to unexpected results, 
in places not foreseen by the programmer(s), therefore the unit tests 
should probe as many of the usage cases as possible while ensuring the 
core functionality remains. 

This directory is the parent directory for unit testing. 

The chosen unit testing framework is minunit as the testing can be done with 
the inclusion of a single header "minunit.h" and only takes a few minutes to 
read up on (see https://github.com/siu/minunit ), which gives some simple 
examples.

Lets say we have a test suite contained in `cleed_example_test_suite.c`, it
can be compiled using::

gcc cleed_example_test_suite.c -lrt -lm -o cleed_example_test_suite.exe 

Note: the `-lrt` is not needed, but provides some information about 
the test suite's running time.