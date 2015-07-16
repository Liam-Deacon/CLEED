# a very simple wrapper around search c interface
if __name__ == '__main__':
    import sys
    from cleed import c_search as csearch
    sys.exit(csearch.main(sys.argv))