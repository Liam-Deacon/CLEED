# a very simple wrapper around ftsmooth c interface
if __name__ == '__main__':
    import sys
    from cleed import c_ftsmooth as ftsmooth
    sys.exit(ftsmooth.main(sys.argv))