# a very simple wrapper around leed c interface
if __name__ == '__main__':
    import sys
    from cleed import c_leed as cleed
    sys.exit(cleed.main(sys.argv))