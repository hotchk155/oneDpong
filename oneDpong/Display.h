/////////////////////////////////////////////////////////////////////////
//
// ONE DIMENSIONAL PONG
// 2015/hotchk155
// Sixty Four Pixels Ltd  six4pix.com/pong1d
//
/////////////////////////////////////////////////////////////////////////

class CDisplay 
{
  public:
    void showDigits(int a, int b, byte shift);
    void showCount(int i);
    void showScores(int a, int b);
    void showText(const char *s, int ofs);     
    int  measureText(const char *sz);

};
extern CDisplay Display;
