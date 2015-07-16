# a very simple wrapper around mkiv c interface
if __name__ == '__main__':
    import sys
    from cleed import c_mkiv as mkiv
    sys.exit(mkiv.main(sys.argv))