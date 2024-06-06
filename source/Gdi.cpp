#include <stdio.h>

#include "Setting.h"
#include "DefOrg.h"
#include "OrgData.h"
#include "Gdi.h"

#define MAXBITMAP		64

HBITMAP hBmp[MAXBITMAP];
HBITMAP hbWork;//������o�b�N�o�b�t�@
HBITMAP hbMparts;//�y���p���[�N
HBITMAP hbPan;//�p���{�����[���p���[�N

HBITMAP waveBmp;

extern int gDrawDouble;	//�����̃g���b�N�O���[�v��`�悷��

extern RECT WinRect; //�E�B���h�E�T�C�Y�ۑ��p A 2010.09.22
extern int NoteWidth;
extern int NoteEnlarge_Until_16px;
extern char* gSelectedTheme;

//GDI�̏�����
BOOL StartGDI(HWND hwnd)
{
	HDC hdc;//�f�o�C�X�R���e�L�X�g
	BOOL status = FALSE;//���̊֐��̕Ԃ�l
    int nDesktopWidth = GetSystemMetrics( SM_CXFULLSCREEN );
    int nDesktopHeight = GetSystemMetrics( SM_CYFULLSCREEN );	//�^�X�N�o�[�l��
    int nScreenWidth = GetSystemMetrics( SM_CXSCREEN );
    int nScreenHeight = GetSystemMetrics( SM_CYSCREEN );


	int nVirtualWidth = WinRect.right - WinRect.left;	//A 2010.09.22
	int nVirtualHeight =WinRect.bottom - WinRect.top;	//A 2010.09.22

	if(nVirtualWidth > nScreenWidth)nScreenWidth = nVirtualWidth;	//A 2010.09.22
	if(nVirtualHeight > nScreenHeight)nScreenHeight = nVirtualHeight;	//A 2010.09.22

	hdc = GetDC(hwnd);//DC�擾
	//�o�b�N�T�[�t�F�X�����ɂ�����
	if((hbWork = CreateCompatibleBitmap(hdc,nScreenWidth,nScreenHeight)) == NULL){
		status = FALSE;
	}
	ReleaseDC(hwnd,hdc);
	return(status);
}
//���T�C�Y���ꂽ�Ƃ��i���s�����֐��j
BOOL ResizeGDI(HWND hwnd)
{
	if(hbWork != NULL)DeleteObject(hbWork);
	HDC hdc;//�f�o�C�X�R���e�L�X�g
	BOOL status = FALSE;//���̊֐��̕Ԃ�l

	hdc = GetDC(hwnd);//DC�擾
	if((hbWork = CreateCompatibleBitmap(hdc,WWidth,WHeight)) == NULL){
		status = FALSE;
	}
	ReleaseDC(hwnd,hdc);
	return(status);

}
//GDI�̊J��
void EndGDI(void)
{
	int i;
	for(i = 0; i < MAXBITMAP; i++){
		if(hBmp[i] != NULL)DeleteObject(hBmp[i]);
	}
	if(hbWork != NULL)DeleteObject(hbWork);
	if(hbMparts != NULL)DeleteObject(hbMparts);
	if(hbPan != NULL)DeleteObject(hbPan);
}
//�摜�̃��[�h(���\�[�X����)
HBITMAP InitBitmap(char *name,int no)
{
	if (hBmp[no] != NULL) DeleteObject(hBmp[no]);
	UINT cap = LR_CREATEDIBSECTION;
	bool useTheme = false;
	char str[MAX_PATH + 20];
	if (strlen(gSelectedTheme) > 0) {
		cap = LR_LOADFROMFILE | LR_CREATEDIBSECTION;
		useTheme = true;
	}
	memset(str, '\0', sizeof(str));

	if (useTheme) sprintf(str, "%s\\%s%s", gSelectedTheme, name, ".bmp");
	else strcpy(str, name);
	hBmp[no] = (HBITMAP)LoadImage(useTheme ? NULL : GetModuleHandle(NULL),
		str,IMAGE_BITMAP,0,0,cap);
	if (hBmp[no] == NULL && useTheme) { // fallback
		hBmp[no] = (HBITMAP)LoadImage(GetModuleHandle(NULL),
			name, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	}
	return hBmp[no];
}
void InitCursor(void)
{
	UINT cap = 0;
	bool useTheme = false;
	char str[MAX_PATH + 20];
	if (strlen(gSelectedTheme) > 0) {
		cap = LR_LOADFROMFILE;
		useTheme = true;
	}
	memset(str, '\0', sizeof(str));

	if (useTheme) sprintf(str, "%s\\CURSOR%s", gSelectedTheme, ".cur");
	else strcpy(str, "CURSOR");
	HCURSOR ccur = (HCURSOR)LoadImage(useTheme ? NULL : GetModuleHandle(NULL),
		str, IMAGE_CURSOR, 0, 0, cap);
	if (ccur == NULL && useTheme) { // fallback
		ccur = (HCURSOR)LoadImage(GetModuleHandle(NULL),
			"CURSOR", IMAGE_CURSOR, 0, 0, 0);
	}
	ccur = (HCURSOR)SetClassLongPtr(hWnd, GCLP_HCURSOR, (LONG)ccur);
	if (ccur != NULL) DestroyCursor(ccur);
}

//������t���b�v
void RefleshScreen(HDC hdc)
{
	while (gIsDrawing) {} // it will prevent incomplete screen from being shown
	HDC hdcwork;//�o�b�N�T�[�t�F�X��DC
	HBITMAP hbold;//�ߋ��̃n���h����ۑ�
	
	hdcwork = CreateCompatibleDC(hdc);//DC�̐���
	hbold = (HBITMAP)SelectObject(hdcwork,hbWork);//�o�b�N�T�[�t�F�X��I��
	//�\��(�t���b�v)
	BitBlt(hdc, 0, 0, WWidth, WHeight,hdcwork,0,0,SRCCOPY);
	SelectObject(hdcwork, hbold);//�I���I�u�W�F�N�g�����ɖ߂�
	DeleteDC(hdcwork);//�f�o�C�X�R���e�L�X�g�̍폜

}

void PutRect(RECT* rect, int color)
{
	HDC hdc, toDC;
	HBITMAP toold;

	hdc = GetDC(hWnd);
	toDC = CreateCompatibleDC(hdc);
	toold = (HBITMAP)SelectObject(toDC, hbWork);
	
	HBRUSH bruh;

	bruh = CreateSolidBrush(color);

	FillRect(toDC, rect, bruh);

	DeleteObject(bruh);
	SelectObject(toDC, toold);
	DeleteDC(toDC);
	ReleaseDC(hWnd, hdc);
}

void PutBitmap(long x,long y, RECT *rect, int bmp_no)
{
	HDC hdc,toDC,fromDC;
	HBITMAP toold,fromold;

	hdc = GetDC(hWnd);
	toDC   = CreateCompatibleDC(hdc);
	fromDC = CreateCompatibleDC(hdc);
	toold   = (HBITMAP)SelectObject(toDC,hbWork);
	fromold = (HBITMAP)SelectObject(fromDC,hBmp[bmp_no]);

	BitBlt(toDC,x,y,rect->right - rect->left,
		rect->bottom - rect->top,fromDC,rect->left,rect->top,SRCCOPY);//�\��

	SelectObject(toDC,toold);
	SelectObject(fromDC,fromold);
	DeleteDC(toDC);
	DeleteDC(fromDC);
	ReleaseDC(hWnd,hdc);
}

void PutBitmapCenter16(long x,long y, RECT *rect, int bmp_no) //���S�ɕ`�悷�� 2014.05.26
{
	if(rect->right - rect->left != 16 || NoteWidth == 16){
		PutBitmap(x, y, rect, bmp_no);
		return;
	}
	HDC hdc,toDC,fromDC;
	HBITMAP toold,fromold;

	hdc = GetDC(hWnd);
	toDC   = CreateCompatibleDC(hdc);
	fromDC = CreateCompatibleDC(hdc);
	toold   = (HBITMAP)SelectObject(toDC,hbWork);
	fromold = (HBITMAP)SelectObject(fromDC,hBmp[bmp_no]);

	int ww = NoteWidth - 4;

	BitBlt(toDC,x     ,y,2   , rect->bottom - rect->top,  fromDC,  rect->left   ,rect->top,SRCCOPY);//�\��
	BitBlt(toDC,x+2   ,y,ww  , rect->bottom - rect->top,  fromDC,  rect->left+2 ,rect->top,SRCCOPY);//�\��
	BitBlt(toDC,x+2+ww,y,2   , rect->bottom - rect->top,  fromDC,  rect->left+14,rect->top,SRCCOPY);//�\��

	SelectObject(toDC,toold);
	SelectObject(fromDC,fromold);
	DeleteDC(toDC);
	DeleteDC(fromDC);
	ReleaseDC(hWnd,hdc);
}
///////////////////////////////////////////////
////�ȍ~�̓��j�[�N�Ί֐�////////////////////////
///////////////////////////////////////////////
//�y���̃p�[�c����
bool MakeMusicParts(unsigned char line,unsigned char dot)
{
	if(line*dot==0)return false;
	if (hbMparts != NULL)DeleteObject(hbMparts);
//	RECT m_rect[] = {
//		{  0,  0, 64,144},//����
//		{ 64,  0, 80,144},//���߃��C��
//		{ 80,  0, 96,144},//�ꔏ���C��
//		{ 96,  0,112,144},//1/16���C��
//	};
	HDC hdc,toDC,fromDC;
	HBITMAP toold,fromold;

	hdc = GetDC(hWnd);

	if ((hbMparts = CreateCompatibleBitmap(hdc, WWidth + ((line * dot + 1) * NoteWidth), 144)) == NULL) {
		ReleaseDC(hWnd, hdc);
		return FALSE;
	}

	toDC   = CreateCompatibleDC(hdc);
	fromDC = CreateCompatibleDC(hdc);
	toold   = (HBITMAP)SelectObject(toDC,hbMparts);
	fromold = (HBITMAP)SelectObject(fromDC,hBmp[BMPMUSIC]);

	int x;
	if(org_data.track>=8)x=0;
	else x=0;

	for(int i = 0; i < (WWidth/NoteWidth)+(line * dot) + 1; i++){ // 15
		if(i%(line*dot) == 0)//��
//			BitBlt(toDC,i*16,0,16,192+WDWHEIGHTPLUS,fromDC,x+64,0,SRCCOPY);//�\��
			BitBlt(toDC,i*NoteWidth,0,NoteWidth,144,fromDC,x+64,0,SRCCOPY);//�\��
		else if(i%dot == 0)//�j��
			BitBlt(toDC,i*NoteWidth,0,NoteWidth,144,fromDC,x+64+16,0,SRCCOPY);//�\��
		else{
			if(NoteWidth>=8){
				BitBlt(toDC,i*NoteWidth,0,NoteWidth,144,fromDC,x+64+32,0,SRCCOPY);//�\��
			}else{
				BitBlt(toDC,i*NoteWidth,0,NoteWidth,144,fromDC,x+64+32+1,0,SRCCOPY);//�\��
			}
		}
	}

	SelectObject(toDC,toold);
	SelectObject(fromDC,fromold);
	DeleteDC(toDC);
	DeleteDC(fromDC);
	ReleaseDC(hWnd,hdc);
	return true;
}

void PutMusicParts(long x,long y)
{
	if (hbPan == NULL) return;

	HDC hdc,toDC,fromDC;
	HBITMAP toold,fromold;

	hdc = GetDC(hWnd);
	toDC   = CreateCompatibleDC(hdc);
	fromDC = CreateCompatibleDC(hdc);
	toold   = (HBITMAP)SelectObject(toDC,hbWork);
	fromold = (HBITMAP)SelectObject(fromDC,hbMparts);

	BitBlt(toDC,x,y,WWidth - x,144,fromDC,0,0,SRCCOPY);//�\��

	SelectObject(toDC,toold);
	SelectObject(fromDC,fromold);
	DeleteDC(toDC);
	DeleteDC(fromDC);
	ReleaseDC(hWnd,hdc);
}
//�p���E�{�����[�����C���\��
void PutPanParts(long x)
{
	if (hbPan == NULL) return;

	HDC hdc,toDC,fromDC;
	HBITMAP toold,fromold;
	int y = WHeight + 288 - WHNM;

	hdc = GetDC(hWnd);
	toDC   = CreateCompatibleDC(hdc);
	fromDC = CreateCompatibleDC(hdc);
	toold   = (HBITMAP)SelectObject(toDC,hbWork);
	fromold = (HBITMAP)SelectObject(fromDC,hbPan);

	BitBlt(toDC,x,y,WWidth - x,144 + 16,fromDC,0,0,SRCCOPY);//�\��

	SelectObject(toDC,toold);
	SelectObject(fromDC,fromold);
	DeleteDC(toDC);
	DeleteDC(fromDC);
	ReleaseDC(hWnd,hdc);
}

void MakePanParts(unsigned char line,unsigned char dot)
{
	if (hbPan != NULL)DeleteObject(hbPan);
//	RECT m_rect[] = {
//		{  0,  0, 64,144},//����
//		{ 64,  0, 80,144},//���߃��C��
//		{ 80,  0, 96,144},//�ꔏ���C��
//		{ 96,  0,112,144},//1/16���C��
//	};
	HDC hdc,toDC,fromDC;
	HBITMAP toold,fromold;

	hdc = GetDC(hWnd);

	if ((hbPan = CreateCompatibleBitmap(hdc, WWidth + ((line * dot + 1) * NoteWidth), 144 + 16)) == NULL) {
		ReleaseDC(hWnd, hdc);
		return;
	}

	toDC   = CreateCompatibleDC(hdc);
	fromDC = CreateCompatibleDC(hdc);
	toold   = (HBITMAP)SelectObject(toDC,hbPan);
	fromold = (HBITMAP)SelectObject(fromDC,hBmp[BMPPAN]);


//	for(int i = 0; i < 40; i++){
	for(int i = 0; i < (WWidth/NoteWidth)+ (line * dot) + 1; i++){ // 15
		if(i%(line*dot) == 0)//��
			BitBlt(toDC,i*NoteWidth,0,NoteWidth,160,fromDC,64,0,SRCCOPY);//�\��
		else if(i%dot == 0)//�j��
			BitBlt(toDC,i*NoteWidth,0,NoteWidth,160,fromDC,64+16,0,SRCCOPY);//�\��
		else {
			if(NoteWidth>=8){
				BitBlt(toDC,i*NoteWidth,0,NoteWidth,160,fromDC,64+32,0,SRCCOPY);//�\��
			}else{
				BitBlt(toDC,i*NoteWidth,0,NoteWidth,160,fromDC,64+32+1,0,SRCCOPY);//�\��
			}
		}
	}

	SelectObject(toDC,toold);
	SelectObject(fromDC,fromold);
	DeleteDC(toDC);
	DeleteDC(fromDC);
	ReleaseDC(hWnd,hdc);
}

void PutSelectParts(long x)
{
	HDC hdc,toDC,fromDC;
	HBITMAP toold,fromold;

	hdc = GetDC(hWnd);
	toDC   = CreateCompatibleDC(hdc);
	fromDC = CreateCompatibleDC(hdc);
	toold   = (HBITMAP)SelectObject(toDC,hbWork);
	fromold = (HBITMAP)SelectObject(fromDC,hbPan);

	BitBlt(toDC,x,WHeight-16,WWidth - x,16,fromDC,0,144,SRCCOPY);//�\��

	SelectObject(toDC,toold);
	SelectObject(fromDC,fromold);
	DeleteDC(toDC);
	DeleteDC(fromDC);
	ReleaseDC(hWnd,hdc);

}

//�ȉ��̓`�g����B������`���Ƃ��݂̂ɗp���邱�ƂƂ���B
HDC		Dw_hdc, Dw_toDC, Dw_fromDC;
HBITMAP Dw_toold, Dw_fromold;

void Dw_BeginToDraw(void)
{
	Dw_hdc = GetDC(hWnd);
	Dw_toDC   = CreateCompatibleDC(Dw_hdc);
	Dw_fromDC = CreateCompatibleDC(Dw_hdc);
	Dw_toold   = (HBITMAP)SelectObject(Dw_toDC,hbWork);
	Dw_fromold = (HBITMAP)SelectObject(Dw_fromDC,hBmp[BMPNOTE]);
	
}

void Dw_FinishToDraw(void)
{
	SelectObject(Dw_toDC,Dw_toold);
	SelectObject(Dw_fromDC,Dw_fromold);
	DeleteDC(Dw_toDC);
	DeleteDC(Dw_fromDC);
	ReleaseDC(hWnd,Dw_hdc);

}

void Dw_PutBitmap(long x,long y, RECT *rect, int bmp_no) //�Ō�̈����͍ő��Ӗ��Ȃ�.
{
	if(NoteWidth == 16){
		BitBlt(Dw_toDC,x,y,rect->right - rect->left,
			rect->bottom - rect->top,Dw_fromDC,rect->left,rect->top,SRCCOPY);//�\��
	}else if(NoteWidth >= 4){ //�Z�k�̏ꍇ
		int ww = NoteWidth - 4;
		BitBlt(Dw_toDC,x,y,2, rect->bottom - rect->top,
			Dw_fromDC,rect->left,rect->top,SRCCOPY);//�\��
		
		if(ww>0){
			BitBlt(Dw_toDC,x+2,y,ww, rect->bottom - rect->top,
				Dw_fromDC,rect->left + 2,rect->top,SRCCOPY);//�\��
		}
		BitBlt(Dw_toDC,x+2+ww,y,2, rect->bottom - rect->top,
			Dw_fromDC,rect->left + 14,rect->top,SRCCOPY);//�\��

	}
}

int Dw_PutBitmap_Head(long x,long y, RECT *rect, int bmp_no, int iNoteLength) //bmp_no�͍ő��Ӗ��Ȃ�. iLength�͕K��1�ȏ�B np->length�����̂܂ܑ�����邱�ƁB
{
	int iTotalLength = NoteWidth * iNoteLength;
	int bitWidth = iTotalLength; if(bitWidth > 16)bitWidth = 16;
	if(NoteEnlarge_Until_16px == 0){bitWidth = NoteWidth; iTotalLength = bitWidth;}

	if(NoteWidth == 16 || iTotalLength >= 16){
		BitBlt(Dw_toDC,x,y,rect->right - rect->left,
			rect->bottom - rect->top,Dw_fromDC,rect->left,rect->top,SRCCOPY);//�\��
		return 16;
	}else if(NoteWidth >= 4){ //�Z�k�̏ꍇ
		int ww = bitWidth - 4;

		BitBlt(Dw_toDC,x,y,2, rect->bottom - rect->top,
			Dw_fromDC,rect->left,rect->top,SRCCOPY);//�\��
		
		if(ww>0){
			BitBlt(Dw_toDC,x+2,y,ww, rect->bottom - rect->top,
				Dw_fromDC,rect->left + 2,rect->top,SRCCOPY);//�\��
		}
		BitBlt(Dw_toDC,x+2+ww,y,2, rect->bottom - rect->top,
			Dw_fromDC,rect->left + 14,rect->top,SRCCOPY);//�\��

	}
	return bitWidth;
}

//PAN, VOL�ɓ������Ă���ȁ[
void Dw_PutBitmap_Center(long x,long y, RECT *rect, int bmp_no) //�Ō�̈����͍ő��Ӗ��Ȃ�.
{
	if(NoteWidth == 16){
		BitBlt(Dw_toDC,x,y,rect->right - rect->left,
			rect->bottom - rect->top,Dw_fromDC,rect->left,rect->top,SRCCOPY);//�\��
	}else if(NoteWidth >= 4){ //�Z�k�̏ꍇ
//		int ww = (16 - NoteWidth) / 2;
//		BitBlt(Dw_toDC,x ,y,rect->right - rect->left - 2 * ww,
//			rect->bottom - rect->top,Dw_fromDC,rect->left + ww,rect->top,SRCCOPY);//�\��
		int ww = NoteWidth / 2;
		BitBlt(Dw_toDC,x ,y,
			ww,
			rect->bottom - rect->top,
			Dw_fromDC,rect->left ,rect->top,SRCCOPY);//�\��

		//BitBlt(Dw_toDC,x+ww-1 ,y-3,
		//	2,
		//	rect->bottom - rect->top,
		//	Dw_fromDC,rect->left+7 ,rect->top,SRCCOPY);//�\��

		BitBlt(Dw_toDC,x+ww ,y,
			ww,
			rect->bottom - rect->top,
			Dw_fromDC,rect->left+16-ww ,rect->top,SRCCOPY);//�\��

		BitBlt(Dw_toDC,x+ww-1 ,y,
			2,
			rect->bottom - rect->top,
			Dw_fromDC,rect->left+7 ,rect->top,SRCCOPY);//�\��

	}
}

void LoadSingleBitmap(HWND hdwnd, int item, int wdth, int hght, const char* name) {
	bool useTheme = (strlen(gSelectedTheme) > 0);

	HANDLE hBmp;
	HANDLE hBmp2;
	char str[MAX_PATH + 20];
	memset(str, '\0', sizeof(str));
	// File name for theme
	if (useTheme) sprintf(str, "%s\\%s%s", gSelectedTheme, name, ".bmp");
	else strcpy(str, name);
	// Load it
	hBmp = (HBITMAP)LoadImage(useTheme ? NULL : hInst, str, IMAGE_BITMAP, wdth, hght, useTheme ? (LR_LOADFROMFILE | LR_DEFAULTCOLOR) : LR_DEFAULTCOLOR);
	if (hBmp == NULL && useTheme) // fallback, if the theme failed
		hBmp = (HBITMAP)LoadImage(hInst, name, IMAGE_BITMAP, wdth, hght, LR_DEFAULTCOLOR);
	// Now set it
	hBmp2 = (HBITMAP)SendDlgItemMessage(hdwnd, item, BM_SETIMAGE, IMAGE_BITMAP, (long)hBmp);
	if (hBmp2 != NULL) DeleteObject(hBmp2);
}

void GenerateWaveGraphic(char *wave100) {
	if (waveBmp != NULL) DeleteObject(waveBmp);

	HBITMAP waveImg = (HBITMAP)LoadImage(hInst, "WAVE100", IMAGE_BITMAP, 377, 491, LR_DEFAULTCOLOR); // BG
	if (waveImg == NULL) return;

	HDC hdc = GetDC(hWnd);
	waveBmp = CreateCompatibleBitmap(hdc, 377, 491);
	if (waveBmp == NULL) {
		ReleaseDC(hWnd, hdc);
		return;
	}

	HDC toDC = CreateCompatibleDC(hdc);
	HDC fromDC = CreateCompatibleDC(hdc);
	HBITMAP toold = (HBITMAP)SelectObject(toDC, waveBmp);
	HBITMAP fromold = (HBITMAP)SelectObject(fromDC, waveImg);

	SelectObject(toDC, GetStockObject(DC_PEN));
	SetDCPenColor(toDC, RGB(0x00, 0xFF, 0x00));

	BitBlt(toDC, 0, 0, 377, 491, fromDC, 0, 0, SRCCOPY);
	for (int i = 0; i < 100; ++i) {
		int x = 8 + (i % 10) * 36 + ((i % 10) > 4 ? 4 : 0); // box position
		int y = 16 + (i / 10) * 48 + ((i / 10) > 4 ? 4 : 0);

		for (int j = 0; j < 256; ++j) {
			int sample = wave100[i * 256 + j]; // Get the sample
			if (j == 0)
				MoveToEx(toDC, x + (j / 8), y + 16 + (-sample / 8), NULL);
			else
				LineTo(toDC, x + (j / 8), y + 16 + (-sample / 8)); // Draw line
		}
	}

	SelectObject(toDC, toold);
	SelectObject(fromDC, fromold);
	DeleteDC(toDC);
	DeleteDC(fromDC);
	ReleaseDC(hWnd, hdc);
	DeleteObject(waveImg);
}


