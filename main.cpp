#include <iostream>
#include <math.h>
#include <time.h>
#include <random>
#include <windows.h>
#include <fstream>
#include <string>

using namespace std;

const int winx=1920;
const int winy=1080;

struct vec3{double x,y,z;};
struct rgb{double r,g,b;};
struct sphere{vec3 pos; double r, diff;rgb clr,eclr; double emmission;};
struct triangle{vec3 A, B, C; rgb clr,eclr; double diff,emmission;vec3 n;};
struct plane{vec3 A, B, C; rgb clr,eclr; double diff,emmission;vec3 n;};
struct playerstats{vec3 pos;};

double a=0;
double b=0;
const int ns=2;
const int nt=1;
const int np=6;

rgb skyclr={0,0,0};

plane pls[]={
    {{-2.5,1,20},{-2.5,1,21},{-2.5,3,22},{1,0,0},{1,0,0},0.05,0,{1,1,1}},
    {{2.5,1,20},{2.5,1,21},{2.5,3,22},{0,1,0},{0,1,0},0.05,0,{1,1,1}},
    {{-1,-2.5,20},{-12,-2.5,22},{-3,-2.5,21},{1,1,1},{1,1,1},0.05,0,{1,1,1}},
    {{-1,2.5,20},{-12,2.5,22},{-3,2.5,21},{1,1,1},{1,1,1},0.05,0,{1,1,1}},
    {{1,2,2.5},{2,3,2.5},{3,1,2.5},{1,1,1},{1,1,1},0.05,0,{1,1,1}},
    {{1,2,-2.5},{2,3,-2.5},{3,1,-2.5},{1,1,1},{1,1,1},0.05,0,{1,1,1}}
};

sphere spheres[]={
    {{0,0,0},0.5,0,{1,1,1},{1,1,1},5},
    {{2,2,1},0.5,0.2,{1,0.1,0.1},{0.1,0.1,0.1},0}
};

triangle triangles[]={{{-1,2,8},{-1,0,8},{-1,0,10},{1,1,1},{1,1,1},0,0,{1,1,1}}};

vec3 rotyx(vec3 pos,double m){
    pos.x=pos.x*cos(a*m)-pos.z*sin(a*m);
    return pos;
}

vec3 rotyz(vec3 pos,double m){
    pos.z=pos.x*sin(a*m)-pos.z*cos(a*m);
    return pos;
}

vec3 rotxy(vec3 pos,double m){
    pos.y=pos.y*cos(b*m)-pos.z*sin(b*m);
    return pos;
}

vec3 rotxz(vec3 pos,double m){
    pos.z=pos.y*sin(b*m)-pos.z*cos(b*m);
    return pos;
}

vec3 rot(vec3 pos,double m){
    vec3 posn;
    posn.x=rotyx(pos,m).x;
    posn.y=rotxy(pos,m).y;
    posn.z=rotyz(rotxz(pos,m),m).z;
    return posn;
}

vec3 norm(vec3 vect){
    double inte=sqrt(vect.x*vect.x+vect.y*vect.y+vect.z*vect.z);
    return {vect.x/inte,vect.y/inte,vect.z/inte};
}

vec3 sub(vec3 v1, vec3 v2){
    return {v1.x-v2.x,v1.y-v2.y,v1.z-v2.z};
}

double dif(vec3 v1, vec3 v2){
    double x=v1.x-v2.x;
    double y=v1.y-v2.y;
    double z=v1.z-v2.z;
    return sqrt(x*x+y*y+z*z);
}

vec3 mul(vec3 v, double a){
    return {v.x*a,v.y*a,v.z*a};
}

double amp(vec3 v){
    return sqrt(v.x*v.x+v.y*v.y+v.z*v.z);
}

double gen(double os){
    static default_random_engine generator(unsigned(time(nullptr)+os));
    uniform_real_distribution<double> distribution(0,1);

    return distribution(generator);
}

vec3 offset(double cof,vec3 dir,double os){
    vec3 v=norm({gen(os),gen(os*12345),gen(os*54321)});
    vec3 newdir={dir.x+(v.x-dir.x)*cof,dir.y+(v.y-dir.y)*cof,dir.z+(v.z-dir.z)*cof};
    return newdir;
}

vec3 cpr(vec3 v1, vec3 v2){
    double p1=-v1.z*v2.y+v1.y*v2.z;
    double p2=v1.z*v2.x-v1.x*v2.z;
    double p3=-v1.y*v2.x+v1.x*v2.y;
    return {p1,p2,p3};
}

double dpr(vec3 v1, vec3 v2){
    return (v1.x*v2.x)+(v1.y*v2.y)+(v1.z*v2.z);
}

rgb colcap(rgb clr){
    return {max(0.0,min(1.0,clr.r)),max(0.0,min(1.0,clr.g)),max(0.0,min(1.0,clr.b))};
}

vec3 add(vec3 v1, vec3 v2){
    return {v1.x+v2.x,v1.y+v2.y,v1.z+v2.z};
}

double intersp(vec3 A, vec3 B, vec3 C, double r){
    double t=dpr(sub(C,A),B);
    vec3 p=add(A,mul(B,t));
    double y=amp(sub(C,p));
    //cout << y << endl;

    if (y<r){
        double x=sqrt(r*r-y*y);
        t=t-x;
        //cout << dif(add(A,mul(B,t)),C)-r << endl;
        return t;
    }
    return 0;
}

vec3 plavg(plane pl){
    return {(pl.A.x+pl.B.x+pl.C.x)/3,(pl.A.y+pl.B.y+pl.C.y)/3,(pl.A.z+pl.B.z+pl.C.z)/3};
}

vec3 triavg(triangle pl){
    return {(pl.A.x+pl.B.x+pl.C.x)/3,(pl.A.y+pl.B.y+pl.C.y)/3,(pl.A.z+pl.B.z+pl.C.z)/3};
}

vec3 trin(plane pl, vec3 p){
    vec3 n=pl.n;
    p=norm(sub(p,plavg(pl)));
    if (dif(p,n)>=1.41421356237)
        return n;
    return mul(n,-1);
}

double det(plane pl, vec3 x){
    vec3 bp=sub(pl.B,pl.A);
    vec3 cp=sub(pl.C,pl.A);
    vec3 xp=sub(x,pl.A);
    double det=bp.x*cp.y*xp.z+bp.y*cp.z*xp.x+bp.z*cp.x*xp.y;
    return det;
}

double interpl(vec3 ray,vec3 dir,plane pl){
    vec3 n;
    n=trin(pl,ray);
    double d=dpr(n,pl.A);
    if (dpr(dir,n)<0.00000001)
        return 0;
    double t=(d-dpr(n,ray))/dpr(n,dir);
    return t;
}

rgb colmul(rgb c1, rgb c2){
    return {c1.r*c2.r,c1.g*c2.g,c1.b*c2.b};
}

rgb coladd(rgb cl1, rgb cl2){
    return {cl1.r+cl2.r,cl1.g+cl2.g,cl1.b+cl2.b};
}

rgb colwg(rgb c1, rgb c2,double step){
    double wg=1/(step+1);
    c1={c1.r*(1-wg),c1.g*(1-wg),c1.b*(1-wg)};
    c2={c2.r*wg,c2.g*wg,c2.b*wg};
    return coladd(c1,c2);
}

rgb trace(playerstats player, double x, double y,double fov){
    vec3 dir;
    vec3 ray;
    double t;
    double d;
    double em;
    rgb aclr;
    rgb clr;
    vec3 n;
    int num=-1;
    int typ=0;
    double d1=(x-double(winx)/2)/(winy*2);
    double d2=(double(winy)/2-y)/(winy*2);
    double d3=sqrt(1-d1*d1-d2*d2)*fov;
    vec3 dirp=rot(norm({d1,d2,d3}),1);
    rgb color={0,0,0};
    int mb=10;
    double tr=5;
    for (double step=0;step<tr;step++){
            clr={1,1,1};
            aclr={0,0,0};
            dir=dirp;
            ray={player.pos.x,player.pos.y,player.pos.z};
            typ=0;
    for (int k=0;k<mb;k++){
            //if (x==winx/2 && y==winy/2) cout << ray.x << " " << ray.y << " " << ray.z << endl;
            d=3000000;
            num=-1;
            for (int i=0;i<ns;i++){
                t=intersp(ray,dir,spheres[i].pos,spheres[i].r);
                if (t>0.00000000001 && t<d){
                    num=i;
                    d=t;
                    typ=1;
                }
            }
            for (int i=0;i<np;i++){
                t=interpl(ray,dir,pls[i]);
                if (t>0.00000001 && t<d){
                    d=t;
                    typ=2;
                    num=i;
                }
            }
            if (aclr.r>0.999 && aclr.g>0.999 && aclr.b>0.999){
                break;
            }
            if (num==-1){
                break;
            }
            else{
                if (typ==1){
                    clr=colmul(clr,spheres[num].clr);
                    em=spheres[num].emmission;
                    aclr=coladd(aclr,colmul(colmul(spheres[num].eclr,{em,em,em}),clr));
                    ray=add(ray,mul(dir,d));
                    n=norm(offset(spheres[num].diff,norm(sub(ray,spheres[num].pos)),step));
                    dir=norm(sub(dir,mul(n,2*dpr(n,dir))));
                }
                else if(typ==2){
                    clr=colmul(clr,pls[num].clr);
                    em=pls[num].emmission;
                    aclr=coladd(aclr,colmul(colmul(pls[num].eclr,{em,em,em}),clr));
                    n=norm(offset(pls[num].diff,trin(pls[num],ray),step));
                    ray=add(ray,mul(dir,d));
                    dir=norm(sub(dir,mul(n,2*dpr(n,dir))));
                }
            }
        }
    color=colwg(color,aclr,step);
    }
    return colcap(color);
}

char* load(char buffer[],playerstats player,double fov){
    for (int i=0;i<np;i++)
        pls[i].n=norm(cpr(sub(pls[i].B,pls[i].A),sub(pls[i].C,pls[i].A)));
    for (int i=0;i<nt;i++)
        triangles[i].n=norm(cpr(sub(triangles[i].B,triangles[i].A),sub(triangles[i].C,triangles[i].A)));
    rgb pixel;
    for (int y=0;y<winy;y++){
        for (int x=0;x<winx;x++){
            pixel=trace(player, x, y, fov);
            int p=y*winx*4+x*4;
            buffer[p] = pixel.b*255;
            buffer[p+1] = pixel.g*255;
            buffer[p+2] = pixel.r*255;
            buffer[p+3] = 0;
        }
    }
    return buffer;
}

void display(char* buffer){
    BITMAP bi;
    HBITMAP bmap=CreateBitmap(winx,winy,1,8*4,(void*) buffer);

    HDC hdc=GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdc);

    SelectObject(hdcMem, bmap); // Inserting picture into our temp HDC
    GetObject((HGDIOBJ)bmap, sizeof(bi), &bi);

    BitBlt(hdc, 0, 0, winx, winy, hdcMem, 0, 0, SRCCOPY);

    DeleteDC(hdcMem); // Deleting temp HDC
    DeleteObject(bmap);
}

bool save(char* buffer, LPCTSTR lpszFileName){
    HBITMAP hBitmap=CreateBitmap(winx,winy,1,8*4,(void*) buffer);
	HDC hDC;
	int iBits;
	WORD wBitCount;
	DWORD dwPaletteSize = 0, dwBmBitsSize = 0, dwDIBSize = 0, dwWritten = 0;
	BITMAP Bitmap0;
	BITMAPFILEHEADER bmfHdr;
	BITMAPINFOHEADER bi;
	LPBITMAPINFOHEADER lpbi;
	HANDLE fh, hDib, hPal, hOldPal2 = NULL;
	hDC = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);
	iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
	DeleteDC(hDC);
	if (iBits <= 1)
		wBitCount = 1;
	else if (iBits <= 4)
		wBitCount = 4;
	else if (iBits <= 8)
		wBitCount = 8;
	else
		wBitCount = 24;
	GetObject(hBitmap, sizeof(Bitmap0), (LPSTR)&Bitmap0);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = Bitmap0.bmWidth;
	bi.biHeight = -Bitmap0.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = wBitCount;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrImportant = 0;
	bi.biClrUsed = 256;
	dwBmBitsSize = ((Bitmap0.bmWidth * wBitCount + 31) & ~31) / 8
		* Bitmap0.bmHeight;
	hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;

	hPal = GetStockObject(DEFAULT_PALETTE);
	if (hPal)
	{
		hDC = GetDC(NULL);
		hOldPal2 = SelectPalette(hDC, (HPALETTE)hPal, FALSE);
		RealizePalette(hDC);
	}


	GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap0.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER)
		+ dwPaletteSize, (BITMAPINFO *)lpbi, DIB_RGB_COLORS);

	if (hOldPal2)
	{
		SelectPalette(hDC, (HPALETTE)hOldPal2, TRUE);
		RealizePalette(hDC);
		ReleaseDC(NULL, hDC);
	}

	fh = CreateFile(lpszFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if (fh == INVALID_HANDLE_VALUE)
		return FALSE;

	bmfHdr.bfType = 0x4D42; // "BM"
	dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;

	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);

	WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);
	GlobalUnlock(hDib);
	GlobalFree(hDib);
	CloseHandle(fh);
	return TRUE;
}

int main(){
    LPTSTR path = "C:\\Users\\User\\Downloads\\raytraced.bmp";

    playerstats player={{0,0,-2.4}};
    char* buffer = new char[winx * winy * 16];
    buffer=load(buffer,player,0.3);
    display(buffer);
    bool sv=save(buffer,path);
    //while (true)
    //    display(buffer);
}
