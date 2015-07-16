# a very simple wrapper around ftsmooth c interface
if __name__ == '__main__':
    import sys
    from cleed import c_latt as latt
    sys.exit(latt.main(sys.argv))