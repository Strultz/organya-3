#include <stdio.h>

#include "Setting.h"
#include "DefOrg.h"
#include "resource.h"
#include "OrgData.h"
#include "Gdi.h"
#include "Timer.h"
#include "Sound.h"
#include "Scroll.h"
char timer_sw = 0;
extern HWND hDlgTrack;
extern HWND hWnd;
extern char* gSelectedTheme;

int mute_name[MAXTRACK] = {
	IDC_MUTE0,
	IDC_MUTE1,
	IDC_MUTE2,
	IDC_MUTE3,
	IDC_MUTE4,
	IDC_MUTE5,
	IDC_MUTE6,
	IDC_MUTE7,
	IDC_MUTE8,
	IDC_MUTE9,
	IDC_MUTE10,
	IDC_MUTE11,
	IDC_MUTE12,
	IDC_MUTE13,
	IDC_MUTE14,
	IDC_MUTE15,
};

void LoadPlayerBitmaps(HWND hdwnd) {
	LoadSingleBitmap(hdwnd, IDC_START, 24, 24, "B_HEAD");
	LoadSingleBitmap(hdwnd, IDC_PLAY, 40, 24, "B_PLAY");
	LoadSingleBitmap(hdwnd, IDC_STOP, 40, 24, "B_STOP");
	LoadSingleBitmap(hdwnd, IDC_LEFT, 24, 24, "B_LEFT");
	LoadSingleBitmap(hdwnd, IDC_RIGHT, 24, 24, "B_RIGHT");
	LoadSingleBitmap(hdwnd, IDC_LEFTSTEP, 24, 24, "B_LEFTSTEP");
	LoadSingleBitmap(hdwnd, IDC_RIGHTSTEP, 24, 24, "B_RIGHTSTEP");
}
//�ȒP�ȃ_�C�A���O�֐�
BOOL CALLBACK DialogPlayer(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam){
	int i;
	HMENU hmenu,hsubmenu;
	MUSICINFO mi;
	HFONT hFont1;
	HWND  hCtrlWnd;
	long hp,vp;
	char str[32];
	switch(message){
	case WM_INITDIALOG://�_�C�A���O���Ă΂ꂽ
		org_data.GetMusicInfo(&mi);
		itoa(mi.wait,str,10);
		SetDlgItemText(hdwnd,IDE_VIEWWAIT,str);
		org_data.SetPlayPointer(0);//���o��
		//�v���C���[�ɕ\��
		//hFont1 = CreateFont(0, 0, 0, 0, 900, 0, 0, 0, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, "�l�r �S�V�b�N");
		hFont1 = CreateFont(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0,
			ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			ANTIALIASED_QUALITY, DEFAULT_PITCH, "Arial Black");
		hCtrlWnd = GetDlgItem(hdwnd, IDE_VIEWMEAS);
		SendMessage(hCtrlWnd, WM_SETFONT, (WPARAM)hFont1, MAKELPARAM(FALSE, 0)); /* �t�H���g�̕ύX */
		SetDlgItemText(hdwnd,IDE_VIEWMEAS,"0");

		hCtrlWnd = GetDlgItem(hdwnd, IDE_VIEWXPOS);
		SendMessage(hCtrlWnd, WM_SETFONT, (WPARAM)hFont1, MAKELPARAM(FALSE, 0)); /* �t�H���g�̕ύX */
		SetDlgItemText(hdwnd,IDE_VIEWXPOS,"0");
		DeleteObject(hFont1); //�t�H���g�I�u�W�F�N�g�̍폜

		//�摜�����
/*
		hBmp = (HBITMAP)LoadImage( hInst, "HEAD", IMAGE_BITMAP, 32, 16, LR_DEFAULTCOLOR );
		SendDlgItemMessage( hdwnd, IDC_START, BM_SETIMAGE, IMAGE_BITMAP, (long)hBmp );
		hBmp = (HBITMAP)LoadImage( hInst, "PLAY", IMAGE_BITMAP, 32, 16, LR_DEFAULTCOLOR );
		SendDlgItemMessage( hdwnd, IDC_PLAY, BM_SETIMAGE, IMAGE_BITMAP, (long)hBmp );
		hBmp = (HBITMAP)LoadImage( hInst, "STOP", IMAGE_BITMAP, 32, 16, LR_DEFAULTCOLOR );
		SendDlgItemMessage( hdwnd, IDC_STOP, BM_SETIMAGE, IMAGE_BITMAP, (long)hBmp );
		hBmp = (HBITMAP)LoadImage( hInst, "LEFT", IMAGE_BITMAP, 32, 16, LR_DEFAULTCOLOR );
		SendDlgItemMessage( hdwnd, IDC_LEFT, BM_SETIMAGE, IMAGE_BITMAP, (long)hBmp );
		hBmp = (HBITMAP)LoadImage( hInst, "RIGHT", IMAGE_BITMAP, 32, 16, LR_DEFAULTCOLOR );
		SendDlgItemMessage( hdwnd, IDC_RIGHT, BM_SETIMAGE, IMAGE_BITMAP, (long)hBmp );
*/
		LoadPlayerBitmaps(hdwnd);
		return 1;
	case WM_MOUSEMOVE:
		if(wParam & MK_LBUTTON){
			ReleaseCapture();
			SendMessage( hdwnd, WM_SYSCOMMAND, SC_MOVE | 2, 0 );

		}
	case WM_COMMAND:
		switch(LOWORD(wParam)){
		case IDC_PLAY:
			if(timer_sw == 0){
				
				//���j���[�𖳌��ɂ���B
				hmenu = GetMenu(hWnd);
				hsubmenu = GetSubMenu(hmenu,0);
				EnableMenuItem(hmenu,0,MF_BYPOSITION|MF_DISABLED);
				EnableMenuItem(hmenu,1,MF_BYPOSITION|MF_DISABLED);
				EnableMenuItem(hmenu,2,MF_BYPOSITION|MF_DISABLED);
				//EnableMenuItem(hmenu,5,MF_BYPOSITION|MF_DISABLED);
				EnableMenuItem(hmenu,4,MF_BYPOSITION|MF_DISABLED);
				EnableMenuItem(hmenu,3,MF_BYPOSITION|MF_DISABLED);
				EnableMenuItem(hmenu,6,MF_BYPOSITION|MF_DISABLED);
				EnableMenuItem(hmenu,7,MF_BYPOSITION|MF_DISABLED);
				EnableMenuItem(hmenu,8,MF_BYPOSITION|MF_DISABLED);
				DragAcceptFiles(hWnd,FALSE);//�h���b�O�֎~
				//�g���b�N �~���[�g �̃`�F�b�N
				for(i = 0; i < MAXTRACK; i++){
					if(SendDlgItemMessage(hDlgTrack,mute_name[i],BM_GETCHECK,0,0)){
						org_data.mute[i] = 1;
					}else{
						org_data.mute[i] = 0;
					}
				}
				//�v���C�|�C���^�[�̐ݒ�
				scr_data.GetScrollPosition(&hp,&vp);
				org_data.SetPlayPointer(hp);
				timer_sw = 1;
				InitMMTimer();
				//�e���|���擾
				MUSICINFO mi;
				org_data.GetMusicInfo(&mi);
				StartTimer(mi.wait);
				SetFocus(hWnd);
			}
			return 1;
		case IDC_STOP:
		case IDC_START:
			if(timer_sw){
				//���j���[��L���ɂ���B
				hmenu = GetMenu(hWnd);
				hsubmenu = GetSubMenu(hmenu,0);
				EnableMenuItem(hmenu,0,MF_BYPOSITION|MF_ENABLED);
				EnableMenuItem(hmenu,1,MF_BYPOSITION|MF_ENABLED);
				EnableMenuItem(hmenu,2,MF_BYPOSITION|MF_ENABLED);
				//EnableMenuItem(hmenu,5,MF_BYPOSITION|MF_ENABLED);
				EnableMenuItem(hmenu,4,MF_BYPOSITION|MF_ENABLED);
				EnableMenuItem(hmenu,3,MF_BYPOSITION|MF_ENABLED);
				EnableMenuItem(hmenu,6,MF_BYPOSITION|MF_ENABLED);
				EnableMenuItem(hmenu,7,MF_BYPOSITION|MF_ENABLED);
				EnableMenuItem(hmenu,8,MF_BYPOSITION|MF_ENABLED);
				DragAcceptFiles(hWnd,TRUE);//�h���b�O����
				org_data.GetMusicInfo(&mi);
				Rxo_StopAllSoundNow();	// 2010.11.30 C
				/*
				for(int i = 0; i < MAXMELODY; i++)
					PlayOrganObject(NULL,2 ,i,NULL);
					*/
				QuitMMTimer();
				timer_sw = 0;
			}
			SetFocus(hWnd);
//			return 1;
//			if(timer_sw){//�X�g�b�v�̌��ʂ�
//				//���j���[��L���ɂ���B
//				hmenu = GetMenu(hWnd);
//				hsubmenu = GetSubMenu(hmenu,0);
//				EnableMenuItem(hmenu,0,MF_BYPOSITION|MF_ENABLED);
//				EnableMenuItem(hmenu,1,MF_BYPOSITION|MF_ENABLED);
//				EnableMenuItem(hmenu,2,MF_BYPOSITION|MF_ENABLED);
//				//EnableMenuItem(hmenu,5,MF_BYPOSITION|MF_ENABLED);
//				EnableMenuItem(hmenu,4,MF_BYPOSITION|MF_ENABLED);
//				EnableMenuItem(hmenu,3,MF_BYPOSITION|MF_ENABLED);
//				EnableMenuItem(hmenu,6,MF_BYPOSITION|MF_ENABLED);
//				EnableMenuItem(hmenu,7,MF_BYPOSITION|MF_ENABLED);
//				EnableMenuItem(hmenu,8,MF_BYPOSITION|MF_ENABLED);
//				DragAcceptFiles(hWnd,TRUE);//�h���b�O����
//				org_data.GetMusicInfo(&mi);
//				Rxo_StopAllSoundNow();	// 2010.11.30 C
//				/*
//				for(int i = 0; i < MAXMELODY; i++)
//					PlayOrganObject(NULL,2 ,i,NULL);
//					*/
//				QuitMMTimer();
//				timer_sw = 0;
//				SetFocus(hWnd);
//			}
			if(LOWORD(wParam) == IDC_START){
//				PlayOrganKey(36,9,1000);
				SetDlgItemText(hdwnd,IDE_VIEWMEAS,"0");
				SetDlgItemText(hdwnd,IDE_VIEWXPOS,"0");
				scr_data.SetHorzScroll(0);
				org_data.SetPlayPointer(0);//���o��
				SetFocus(hWnd);
			}
			return 1;
		case IDC_LEFT:
			scr_data.HorzScrollProc(SB_PAGELEFT);
			SetFocus(hWnd);
			return 1;
		case IDC_RIGHT:
			scr_data.HorzScrollProc(SB_PAGERIGHT);
			SetFocus(hWnd);
			return 1;
		case IDC_LEFTSTEP:
			scr_data.HorzScrollProc(SB_LINELEFT);
			SetFocus(hWnd);
			return 1;
		case IDC_RIGHTSTEP:
			scr_data.HorzScrollProc(SB_LINERIGHT);
			SetFocus(hWnd);
			return 1;
		}
		//case WM_PAINT:
		//	break;
			/*
			HDC hdc;
			PAINTSTRUCT ps;
			RECT rc1={0,0,200,36},rc2={0,0,1,36},rc3={0,0,200,1};
			HBRUSH brh1,brh2;
			hdc = BeginPaint(hdwnd,&ps);
			brh1 = CreateSolidBrush(0x323232); //0x00bbggrr 
			brh2 = CreateSolidBrush(0x808080); //0x00bbggrr 
			FillRect(hdc, &rc1, brh1);
			FillRect(hdc, &rc2, brh2);
			FillRect(hdc, &rc3, brh2);

			DeleteObject(brh1);
			EndPaint(hWnd,&ps);
			*/
		//	break;
	case WM_CLOSE:
		//DeleteObject(hFont1); //�t�H���g�I�u�W�F�N�g�̍폜
		return 1;
	}
	return 0;
}