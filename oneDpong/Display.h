class CDisplay 
{
  public:
    void showDigits(int a, int b, byte shift);
    void showCount(int i);
    void showScores(int a, int b);
    void showText(const char *s, int ofs);     
};
extern CDisplay Display;
