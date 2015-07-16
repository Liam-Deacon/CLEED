# a very simple wrapper around crfac extension
if __name__ == '__main__':
    import sys
    from cleed import c_rfac as crfac
    sys.exit(crfac.main(sys.argv))
