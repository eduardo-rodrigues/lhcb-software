#ifndef BUFFERMANAGER_BITS_H
#define BUFFERMANAGER_BITS_H
extern "C" {
#ifdef __cplusplus
#endif
  inline int bit_set(unsigned int* mask,int pos)  {
    mask[pos/32] |= (1<<(pos%32));
    return 0;
  }

  inline int bit_clear(unsigned int* mask,int pos)  {
    mask[pos/32] &= ~(1<<(pos%32));
    return 0;
  }

  inline int bit_test (const unsigned int *mask,int pos)    {
    return (mask[pos/32] & (1<<(pos%32)));
  }
  int mask_and    (const unsigned int* mask1,const unsigned int* mask2,unsigned int* mask3,int mask_size);
  int mask_or     (const unsigned int* mask1, const unsigned int* mask2,unsigned int* mask3,const int mask_size);
  int mask_and_ro (const unsigned int* mask1,const unsigned int* mask2,const int mask_size);
  int mask_or_ro  (const unsigned int* mask1,const unsigned int* mask2, const int mask_size);
  int mask_summ   (const unsigned int* mask, int mask_size);
  int BF_alloc    (char *base, int bf_size,int size_wanted,int* pos_found);
  int BF_count    (const char* base,int bf_size,int* pos,int* size);
  int BF_set      (char* base, int start,int end);
  int BF_free     (char* base,int pos, int size);
#ifdef __cplusplus
};
namespace Bits  {
  template <int i> struct BitMask  {
    unsigned int m_mask[i];
    BitMask() {
    }
    BitMask(int def) {
      for(int j=0; j<i;++j) m_mask[j]=def;
    }
    void clear()  {
      ::memset(m_mask, 0, i * sizeof(m_mask[0]));
    }
    int mask_or_ro(const BitMask<i>& mask)  const   {
      return ::mask_or_ro(m_mask, mask.m_mask, i);
    }
    int mask_summ()  const    {
      return ::mask_summ(m_mask,i);
    }
    int mask_and(const BitMask<i>& mask1, const BitMask<i>& mask2)  {
      return ::mask_and(mask1.m_mask, mask2.m_mask, m_mask, i);
    }
    void bit_set(int which)  {
      ::bit_set(m_mask,which);
    }
    void bit_clear(int which)  {
      ::bit_clear(m_mask,which);
    }
    int bit_test(int which) const  {
      return ::bit_test(m_mask,which);
    }
    BitMask& operator=(const int val[i]) {
      for(int j=0; j<i;++j) m_mask[j]=val[j];
      return *this;
    }
    BitMask& operator=(const BitMask<i>& val) {
      for(int j=0; j<i;++j) m_mask[j]=val.m_mask[j];
      return *this;
    }
    bool operator==(const int val[i]) {
      return ::memcmp(val,m_mask,i*sizeof(m_mask[0])) == 0;
    }
    bool operator==(const BitMask<i>& val) {
      return ::memcmp(val.m_mask,m_mask,i*sizeof(m_mask[0])) == 0;
    }
    bool operator!=(const int val[i]) {
      return ::memcmp(val,m_mask,i*sizeof(m_mask[0])) != 0;
    }
    bool operator!=(const BitMask<i>& val) {
      return ::memcmp(val.m_mask,m_mask,i*sizeof(m_mask[0])) != 0;
    }
    const unsigned int* bits()  const  {
      return m_mask;
    }
    const unsigned int word(int which)  const  {
      return m_mask[which];
    }
    void setWord(int which, int value)  {
      m_mask[which] = value;
    }
  };
}
#endif

#endif // BUFFERMANAGER_BITS_H
