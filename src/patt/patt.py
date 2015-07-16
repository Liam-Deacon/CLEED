# a very simple wrapper around patt c interface
if __name__ == '__main__':
    import sys
    from cleed import c_patt as cpatt
    sys.exit(cpatt.main(sys.argv))