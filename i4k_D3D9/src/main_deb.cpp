//--------------------------------------------------------------------------//
// iq / rgba  .  tiny codes  .  2008                                        //
//--------------------------------------------------------------------------//

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#include <mmsystem.h>
#include <d3d9.h>
#include <dsound.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "intro.h"
#include "mzk.h"
#include "config.h"

static D3DPRESENT_PARAMETERS devParams = 
{
    0, 0,   // xres, yres
    D3DFMT_A8R8G8B8, 
    0,      // backBufferCount;
    D3DMULTISAMPLE_4_SAMPLES, 
    0,      // multiSampleQuality;
    D3DSWAPEFFECT_DISCARD, 
    0,      // hDeviceWindow;
    true,   // windowed
    true,
    D3DFMT_D24S8,
    0,  // flags;
    0,  // fullScreen_RefreshRateInHz;
    D3DPRESENT_INTERVAL_IMMEDIATE   // presentationInterval
};

//----------------------------------------------------------------------------

typedef struct
{
    //---------------
    HINSTANCE   hInstance;
    HDC         hDC;
    HWND        hWnd;
    //---------------
    int         full;
    //---------------
    char        wndclass[4];	// window class and title :)
    //---------------
}WININFO;


static WININFO wininfo = {  0,0,0,0, {'i','q','_',0} };

//==============================================================================================


static LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// salvapantallas
	if( uMsg==WM_SYSCOMMAND && (wParam==SC_SCREENSAVE || wParam==SC_MONITORPOWER) )
		return( 0 );

	// boton x o pulsacion de escape
	if( uMsg==WM_CLOSE || uMsg==WM_DESTROY || (uMsg==WM_KEYDOWN && wParam==VK_ESCAPE) )
		{
		PostQuitMessage(0);
        return( 0 );
		}

    if( uMsg==WM_CHAR || uMsg==WM_KEYDOWN)
        {
        if( wParam==VK_ESCAPE )
            {
            PostQuitMessage(0);
            return( 0 );
            }
        }

    return( DefWindowProc(hWnd,uMsg,wParam,lParam) );
}


static void window_end( WININFO *info )
{
    if( info->hDC  ) ReleaseDC( info->hWnd, info->hDC );
    if( info->hWnd ) DestroyWindow( info->hWnd );

    UnregisterClass( info->wndclass, info->hInstance );

    if( info->full )
        {
        ChangeDisplaySettings( 0, 0 );
		while( ShowCursor( 1 )<0 ); // show cursor
        }
}

//-----------------------------------------------
//-----------------------------------------------

static int window_init( WININFO *info )
{
    DWORD			dwExStyle, dwStyle;
    DEVMODE			dmScreenSettings;
    RECT			rec;
    WNDCLASS		wc;

    ZeroMemory( &wc, sizeof(WNDCLASS) );
    wc.style         = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = info->hInstance;
    wc.lpszClassName = info->wndclass;
    wc.hbrBackground=(HBRUSH)CreateSolidBrush(0x00785838);
	
    if( !RegisterClass(&wc) )
        return( 0 );

    if( info->full )
    {
        dmScreenSettings.dmSize       = sizeof(DEVMODE);
        dmScreenSettings.dmFields     = DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;
        dmScreenSettings.dmBitsPerPel = 32;
        dmScreenSettings.dmPelsWidth  = XRES;
        dmScreenSettings.dmPelsHeight = YRES;

        if( ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
            return( 0 );

        dwExStyle = WS_EX_APPWINDOW;
        dwStyle   = WS_VISIBLE | WS_POPUP;// | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

		while( ShowCursor( 0 )>=0 );	// hide cursor
    }
    else
    {
        dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
        dwStyle   = WS_VISIBLE | WS_CAPTION | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU;
    }

    rec.left   = 0;
    rec.top    = 0;
    rec.right  = XRES;
    rec.bottom = YRES;

    AdjustWindowRect( &rec, dwStyle, 0 );

    info->hWnd = CreateWindowEx( dwExStyle, wc.lpszClassName, "alohamora", dwStyle,
                               (GetSystemMetrics(SM_CXSCREEN)-rec.right+rec.left)>>1,
                               (GetSystemMetrics(SM_CYSCREEN)-rec.bottom+rec.top)>>1,
                               rec.right-rec.left, rec.bottom-rec.top, 0, 0, info->hInstance, 0 );

    if( !info->hWnd )
        return( 0 );

    info->hDC = GetDC(info->hWnd);
    if( !info->hDC )
        return( 0 );
    
    return( 1 );
}


//==============================================================================================

#ifdef USEDSOUND
static WAVEFORMATEX format={WAVE_FORMAT_PCM, MZK_NUMCHANNELS, MZK_RATE, MZK_RATE*MZK_NUMCHANNELS*sizeof(short), MZK_NUMCHANNELS*sizeof(short), 8*sizeof(short)};       
static DSBUFFERDESC bufferDesc1={sizeof(DSBUFFERDESC), DSBCAPS_PRIMARYBUFFER};
static DSBUFFERDESC bufferDesc2={sizeof(DSBUFFERDESC), DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_GLOBALFOCUS, MZK_NUMSAMPLESC*sizeof(short), NULL, &format, NULL };
#else
static const int wavHeader[11] = {
    0x46464952, 
    MZK_NUMSAMPLES*2+36, 
    0x45564157, 
    0x20746D66, 
    16, 
    WAVE_FORMAT_PCM|(MZK_NUMCHANNELS<<16), 
    MZK_RATE, 
    MZK_RATE*MZK_NUMCHANNELS*sizeof(short), 
    (MZK_NUMCHANNELS*sizeof(short))|((8*sizeof(short))<<16),
    0x61746164, 
    MZK_NUMSAMPLES*sizeof(short)
    };
static short myMuzik[MZK_NUMSAMPLESC+22];
#endif

int WINAPI WinMain( HINSTANCE instance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    MSG         msg;
    int         done=0;
    WININFO     *info = &wininfo;

	IDirect3DDevice9 *d3dDevice;

	IDirect3D9 *d3d = Direct3DCreate9( D3D_SDK_VERSION );
	if( !d3d )
		return 0;

    info->hInstance = GetModuleHandle( 0 );

    //if( MessageBox( 0, "fullscreen?", info->wndclass, MB_YESNO|MB_ICONQUESTION)==IDYES ) info->full++;

    if( !window_init(info) )
        {
        window_end( info );
        MessageBox( 0, "window_init()!","error",MB_OK|MB_ICONEXCLAMATION );
        return( 0 );
        }

    devParams.hDeviceWindow = info->hWnd;
    if( d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, devParams.hDeviceWindow,
                           D3DCREATE_HARDWARE_VERTEXPROCESSING, &devParams, &d3dDevice ) < 0 )
        return 0;

    intro_init( d3dDevice );

    #ifdef USEDSOUND
    short                *myMuzik;
    unsigned long        length;
    LPDIRECTSOUNDBUFFER  m_pSecondary;
    LPDIRECTSOUND        m_pDS;
    LPDIRECTSOUNDBUFFER  m_pPrimary;
    DirectSoundCreate(0, &m_pDS, 0);
    m_pDS->SetCooperativeLevel(devParams.hDeviceWindow,DSSCL_PRIORITY);
    m_pDS->CreateSoundBuffer(&bufferDesc1,&m_pPrimary, NULL);
    m_pDS->CreateSoundBuffer(&bufferDesc2,&m_pSecondary,NULL);
    m_pPrimary->SetFormat(&format);
    m_pSecondary->Lock(0,MZK_NUMSAMPLESC*sizeof(short),(void**)&myMuzik,&length,NULL,NULL,NULL);
    #endif

    mzk_init(myMuzik);

    #ifdef USEDSOUND
    m_pSecondary->Unlock(myMuzik,length,NULL,NULL);
    m_pSecondary->Play(0,0,0);
    #else
    memcpy( myMuzik, wavHeader, 44 );
    sndPlaySound( (const char*)&myMuzik, SND_ASYNC|SND_MEMORY );
    #endif

    long to=timeGetTime();
    while( !done )
    {
        while( PeekMessage(&msg,0,0,0,PM_REMOVE) )
        {
            if( msg.message==WM_QUIT ) done=1;
		    TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        #ifdef USEDSOUND
        m_pSecondary->GetCurrentPosition(&t,NULL);
        unsigned long t = 10*t/882;
        #else
        long t = timeGetTime()-to; 
        #endif
        intro_do( d3dDevice, t );
    }

    #ifndef USEDSOUND
    sndPlaySound(0,0);
    #endif

    d3dDevice->Release();
    d3d->Release();
    window_end( info );

    return( 0 );
}



