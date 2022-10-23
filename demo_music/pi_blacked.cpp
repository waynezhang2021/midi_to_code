#include<windows.h>
#include<math.h>
#include<cstdio>
using namespace std;
double pi=3.1415926535897932384626433832795;
int data_size=93086;
HWAVEOUT out;
WAVEFORMATEX waveform;
int bufsize=8146607;
WAVEHDR header;
int main()
{
	short* pcm=new short[bufsize];
	waveform.wFormatTag=WAVE_FORMAT_PCM;
	waveform.nSamplesPerSec=44100;
	waveform.wBitsPerSample=16;
	waveform.nChannels=1;
	waveform.nAvgBytesPerSec=2*44100;
	waveform.nBlockAlign=2;
	waveform.cbSize=0;

	HANDLE wait=CreateEvent(NULL,0,0,"");

	header.lpData=(LPSTR)pcm;
	header.dwBufferLength=bufsize*2;
	header.dwBytesRecorded=0;
	header.dwUser=0;
	header.dwFlags=WAVE_ALLOWSYNC;
	header.dwLoops=1;
	printf("generating PCM...");
	for(int i=0;i<data_size;i++)
		for(int j=data[i*3+1];j<data[i*3+2];j++)
			pcm[j]+=(sin(pi/44100*data[i*3]*(j-data[i*3+1]))+1)*64;
	printf("done");
	waveOutOpen(&out,WAVE_MAPPER,&waveform,(DWORD_PTR)&wait,0,CALLBACK_EVENT);
	waveOutPrepareHeader(out,&header,sizeof(WAVEHDR));
	waveOutWrite(out,&header,sizeof(WAVEHDR));
	Sleep(bufsize/44.100);
	waveOutClose(out);
	return 0;
}