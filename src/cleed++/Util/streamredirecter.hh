#ifndef STREAMREDIRECTER_HH
#define STREAMREDIRECTER_HH

#include <ostream>

/*! 
 * Constructing an instance of this class causes
 * anything written to the source stream to be redirected
 * to the destination stream.
 */
class StreamRedirecter
{
public:
  StreamRedirecter(std::ostream & dst, std::ostream & src)
    : src(src), srcbuf(src.rdbuf())
  {
    src.rdbuf(dst.rdbuf());
  }
  
  // The destructor restores the original source stream buffer
  ~StreamRedirecter()
  {
    src.rdbuf(srcbuf);
  }
private:
  std::ostream &src;
  std::streambuf *const srcbuf;
};

#endif /* STREAMREDIRECTER_HH */