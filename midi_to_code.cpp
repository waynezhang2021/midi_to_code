#pragma once
#include<bits/stdc++.h>
#include<pthread.h>
#include<string.h>
#include<csignal>
#include<cstdlib>
#include<vector>
#include<bitset>
#include<string>
#include<cstdio>
#include<queue>
#include<stack>
using namespace std;
void info(int argc,char** argv)
{
	if(argc==1)
	{
		printf("MIDI to dynamically computed PCM(pulse code modulation) C++ code tool.\nusage:%s <csv_file> <volume> <transpose_ratio>",argv[0]);
		exit(0);
	}
	if(argc==4)
		return;
	if(argc>=4||argc==2||argc==3)
	{
		printf("invalid number of parameters(should be 3)");
		exit(0);
	}
}
double process_note(string note)
{
	static int tbl[]= {1,3,5,6,8,10,12};
	double base=261.6255653005986346778499935234;
	double ratio=1.0594630943592952645618252949463;
	if(note[0]>='C'&&note[0]<='G')
		base*=pow(ratio,tbl[note[0]-'C']);
	else
		base*=pow(ratio,tbl[((note[0]=='A')?5:6)]);
	if(note[1]=='#')
	{
		base*=ratio;
		base*=pow(2,atoi(note.substr(2).c_str()));
		return base;
	}
	else
	{
		base*=pow(2,atoi(note.substr(1).c_str()));
		return base;
	}
}
void process_midi(string file,int volume,double transpose)
{
	if(file.substr(file.length()-4,4)!=".csv")
	{
		printf("the specified file is not a CSV file");
		exit(0);
	}
	ifstream in(file);
	FILE *out=fopen((file.substr(0,file.length()-4)+".cpp").c_str(),"w");
	if(!in)
	{
		printf("failed to open input file");
		exit(0);
	}
	if(!out)
	{
		printf("failed to open output file");
		exit(0);
	}
	string useless_str;
	float useless_flt;
	in>>useless_str>>useless_str>>useless_str>>useless_str>>useless_str;
	float music_length;
	while(in>>useless_str>>useless_flt>>music_length>>useless_flt>>useless_flt);
	in.clear();
	in.seekg(0);
	printf("detected music length:%fs\n",music_length);
	string note;
	float start,end;
	double freq;
	int data_size=0;
	printf("process notes...");
	in>>useless_str>>useless_str>>useless_str>>useless_str>>useless_str;
	fprintf(out,"#include<windows.h>\n#include<math.h>\n#include<cstdio>\nusing namespace std;\ndouble pi=3.1415926535897932384626433832795;\nfloat data[]={");
	while(in>>note>>start>>end>>useless_flt>>useless_flt)
	{
		freq=process_note(note);
		fprintf(out,"%lf,",freq*transpose);
		fprintf(out,"%d,",int(start*44100));
		fprintf(out,"%d,",int(end*44100));
		data_size++;
	}
	in.close();
	fseek(out,-1,SEEK_CUR);
	fprintf(out,"};\n");
	fprintf(out,"int data_size=%d;",data_size);
	printf("done\n");
	printf("output program...");
	fprintf(out,"\nHWAVEOUT out;\nWAVEFORMATEX waveform;\nint bufsize=%d;\nWAVEHDR header;\nint main()\n{\n\tshort* pcm=new short[bufsize];\n",int(44100*music_length)+1);
	fprintf(out,"\twaveform.wFormatTag=WAVE_FORMAT_PCM;\n\twaveform.nSamplesPerSec=44100;\n\twaveform.wBitsPerSample=16;\n\twaveform.nChannels=1;\n");
	fprintf(out,"\twaveform.nAvgBytesPerSec=2*44100;\n\twaveform.nBlockAlign=2;\n\twaveform.cbSize=0;\n\n");
	fprintf(out,"\tHANDLE wait=CreateEvent(NULL,0,0,\"\");\n\n");
	fprintf(out,"\theader.lpData=(LPSTR)pcm;\n\theader.dwBufferLength=bufsize*2;\n\theader.dwBytesRecorded=0;\n\theader.dwUser=0;\n");
	fprintf(out,"\theader.dwFlags=WAVE_ALLOWSYNC;\n\theader.dwLoops=1;\n\tprintf(\"generating PCM...\");\n");
	fprintf(out,"\tfor(int i=0;i<data_size;i++)\n\t\tfor(int j=data[i*3+1];j<data[i*3+2];j++)\n\t\t\tpcm[j]+=(sin(pi/44100*data[i*3]*(j-data[i*3+1]))+1)*%d;\n",volume);
	fprintf(out,"\tprintf(\"done\");\n\twaveOutOpen(&out,WAVE_MAPPER,&waveform,(DWORD_PTR)&wait,0,CALLBACK_EVENT);\n");
	fprintf(out,"\twaveOutPrepareHeader(out,&header,sizeof(WAVEHDR));\n\twaveOutWrite(out,&header,sizeof(WAVEHDR));\n\tSleep(bufsize/44.100);\n");
	fprintf(out,"\twaveOutClose(out);\n\treturn 0;\n}");
	fclose(out);
	printf("done");
	return;
}
int main(int argc,char** argv)
{
	info(argc,argv);
	process_midi(argv[1],atoi(argv[2]),stod(argv[3]));
}
