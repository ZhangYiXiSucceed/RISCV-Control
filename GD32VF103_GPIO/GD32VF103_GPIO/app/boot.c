__attribute__((section(".boot")))

extern void* _ilm_lma; 
extern void* _ilm; 
extern void* _eilm;
void copy(void)
{
    volatile  char *pa,*pb;
    unsigned int i=0;
    pa=(unsigned char *)(&_ilm_lma);
    pb=(unsigned char *)(&_ilm);

    int text_size = (int)((char*)&_eilm-(char*)&_ilm);
    for(i=0;i<(text_size);i++)
    {
        *pb = *(volatile char*)pa;
        pb++;
        pa++;
    }
}

