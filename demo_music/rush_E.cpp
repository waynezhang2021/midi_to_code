#include<windows.h>
#include<math.h>
#include<cstdio>
using namespace std;
double pi=3.1415926535897932384626433832795;
HWAVEOUT out;
WAVEFORMATEX waveform;
int bufsize=6137680;
WAVEHDR header;
int main()
{
	short* pcm=new short[bufsize];
	memset(pcm,0,bufsize*2);
	waveform.wFormatTag=WAVE_FORMAT_PCM;
	waveform.nSamplesPerSec=44100;
	waveform.wBitsPerSample=16;
	waveform.nChannels=1;
	waveform.nAvgBytesPerSec=2*44100;
	waveform.nBlockAlign=2;
	waveform.cbSize=0;
	header.lpData=(LPSTR)pcm;
	header.dwBufferLength=bufsize*2;
	header.dwBytesRecorded=0;
	header.dwUser=0;
	header.dwFlags=WAVE_ALLOWSYNC;
	header.dwLoops=1;
	printf("reading PCM...");
	FILE* f=fopen("rush_E.pcm","rb");
	fread(pcm,bufsize*2,1,f);
	printf("done");
	waveOutOpen(&out,WAVE_MAPPER,&waveform,(DWORD_PTR)nullptr,0,CALLBACK_NULL);
	waveOutPrepareHeader(out,&header,sizeof(WAVEHDR));
	waveOutWrite(out,&header,sizeof(WAVEHDR));
	Sleep(bufsize/44.100);
	waveOutClose(out);
	return 0;
}
