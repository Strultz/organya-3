#include <windows.h>
#define MAXBITMAP		64

//void PutMusic(void);
//GDI�̏�����
BOOL StartGDI(HWND hwnd);

BOOL ResizeGDI(HWND hwnd);

//GDI�̊J��
void EndGDI(void);
//�摜�̃��[�h(���\�[�X����)
HBITMAP InitBitmap(char *name,int bmp_no);
void InitCursor();
//������t���b�v
void RefleshScreen(HDC hdc);
void PutRect(RECT* rect, int color);
void PutBitmap(long x,long y, RECT *rect, int bmp_no);
void PutBitmapCenter16(long x,long y, RECT *rect, int bmp_no);
////////////////////////////////
///�ȉ��̓��j�[�N�Ȋ֐�//////////
bool MakeMusicParts(unsigned char line,unsigned char dot);
void PutMusicParts(long x,long y);
void PutPanParts(long x);
void MakePanParts(unsigned char line,unsigned char dot);

void PutSelectParts(long x);

//�ȉ��̓`�g����B������`���Ƃ��݂̂ɗp���邱�ƂƂ���B
void Dw_BeginToDraw(void);
void Dw_FinishToDraw(void);
void Dw_PutBitmap(long x,long y, RECT *rect, int bmp_no);
int  Dw_PutBitmap_Head(long x,long y, RECT *rect, int bmp_no, int iNoteLength);
void Dw_PutBitmap_Center(long x,long y, RECT *rect, int bmp_no); //2014.05.26 A

void LoadSingleBitmap(HWND hdwnd, int item, int wdth, int hght, const char* name);
void GenerateWaveGraphic(char *wave100);

extern bool gIsDrawing;

