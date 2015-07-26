
class CMatrix {
  byte m_buffer1[16];
  byte m_buffer2[16];
public:
  byte *m_DispBuffer;
  byte *m_DrawBuffer;
  byte m_Switch;
  CMatrix() {
    memset(m_buffer1, 0, 16);
    memset(m_buffer2, 0, 16);
    m_DispBuffer = m_buffer1;
    m_DrawBuffer = m_buffer2;
  }
  void flip() {
    m_Switch = 1;
  }
  void getDispBuffer() {
    memcpy(m_DrawBuffer, m_DispBuffer, 16);
  }
  void clearDrawBuffer() {
    memset(m_DrawBuffer, 0, 16);
  }
};


extern CMatrix Matrix;

