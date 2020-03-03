/* This program is free software; you can redistribute it and/or modify it under the terms of teh GNU General Public LIcense as published by the Free Software Foundation; either version 2 of teh License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

Copyright (c) 2003 Tyler Knott, 2020 Sarami*/
#pragma warning(disable:4514 4710 4711)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma warning(disable:4996 5045)

#define UNREFERENCED_PARAMETER(P) (P)

int split16(char* rawfile)
{
	FILE * fin;
	FILE * foutr;
	FILE * foutl;
	char l1;
	char l2;
	char r1;
	char r2;
	long finSize;
	printf("Split16 by Tyler Knott\n Splits stereo 16-bit files into two mono 16-bit files.  Pretty much useless outside of PVDTools.\n");
	fin = fopen(rawfile, "rb");
	if (!fin) { printf("No input!!!"); exit(2); }
	foutl = fopen("input-raw-left.raw", "wb");
	foutr = fopen("input-raw-right.raw", "wb");
	fseek(fin, 0, 2);
	finSize = ftell(fin);
	fseek(fin, 0, 0);
	for (finSize = finSize; finSize != 0; finSize = finSize - 4)
	{
		fread(&l1, 1, 1, fin);
		fread(&l2, 1, 1, fin);
		fread(&r1, 1, 1, fin);
		fread(&r2, 1, 1, fin);
		fwrite(&l1, 1, 1, foutl);
		fwrite(&l2, 1, 1, foutl);
		fwrite(&r1, 1, 1, foutr);
		fwrite(&r2, 1, 1, foutr);
	}
	fclose(fin);
	fclose(foutr);
	fclose(foutl);
	return 0;
}

int split8()
{
	FILE * fin;
	FILE * foutr;
	FILE * foutl;
	char l1;
//	char l2;
	char r1;
//	char r2;
	long finSize;
	printf("Split8 by Tyler Knott\n Splits 8-bit stereo tracks into 2 8-bit mono tracks, skipping the first 2531 bytes.  Largely useless outside of the PVDTools pack.\n");
	fin = fopen("input-raw-right.raw", "rb");
	if (!fin) { printf("No input!!!"); exit(2); }
	foutl = fopen("input-raw-left2.raw", "wb");
	foutr = fopen("input-raw-right2.raw", "wb");
	fseek(fin, 0, 2);
	finSize = ftell(fin);
//	printf("File size: %d\n", finSize);
	fseek(fin, 0, 0);
	for (finSize = finSize; finSize != 0; finSize = finSize - 2)
	{
		fread(&l1, 1, 1, fin);
		/* fread(&l2, 1, 1, fin); */
		fread(&r1, 1, 1, fin);
		/* fread(&r2, 1, 1, fin); */
		fwrite(&l1, 1, 1, foutl);
		/*  fwrite(&l2,1,1,foutl); */
		fwrite(&r1, 1, 1, foutr);
		/*  fwrite(&r2,1,1,foutr); */
	}
	fclose(fin);
	fclose(foutr);
	fclose(foutl);
	remove("input-raw-right.raw");
	return 0;
}

int addh()
{
	FILE * fin;
	FILE * fout;
	long finSize;
	char * buffer;
	fin = fopen("input-raw-right2.raw", "rb");
	if (!fin) { printf("ERROR!"); exit(2); }
	char riff[] = "RIFF";
	char wave[] = "WAVE";
	char fmt[] = "fmt ";
	long sc1size = 16;
	int audiofmt = 1;
	int numchanels = 1;
	long smprate = 44100;
	long byterate = 44100;
	int blkalign = 1;
	int bits = 8;
	char dataa[] = "data";
	fout = fopen("input-right2.wav", "wb");
	fwrite(riff, 4, 1, fout);
	fseek(fin, 0, 2);
//	finSize = ftell(fin) - 2531;
//	fseek(fin, 2531, 0);
	finSize = ftell(fin);
	fseek(fin, 0, 0);
	long finSizea;
	finSizea = finSize + 40;
	fwrite(&finSizea, 4, 1, fout);
	fwrite(wave, 4, 1, fout);
	fwrite(fmt, 4, 1, fout);
	fwrite(&sc1size, 4, 1, fout);
	fwrite(&audiofmt, 2, 1, fout);
	fwrite(&numchanels, 2, 1, fout);
	fwrite(&smprate, 4, 1, fout);
	fwrite(&byterate, 4, 1, fout);
	fwrite(&blkalign, 2, 1, fout);
	fwrite(&bits, 2, 1, fout);
	fwrite(dataa, 4, 1, fout);
	fwrite(&finSize, 4, 1, fout);
	buffer = (char*)malloc((size_t)finSize);
	if (buffer) {
		fread(buffer, 1, (size_t)finSize, fin);
		fwrite(buffer, 1, (size_t)finSize, fout);
	}
	fclose(fin);
	fclose(fout);
	remove("input-raw-right2.raw");
	return 0;
}

int isHeader(unsigned char buf)
{
	int nRet = 0;
	if (buf == 0xe1 || buf == 0xc3 || buf == 0xa5) {
		nRet = 1;
	}
	return nRet;
}

int isFooter(unsigned char buf)
{
	int nRet = 0;
	if (buf == 0xd2 || buf == 0xb4 || buf == 0x96) {
		nRet = 1;
	}
	return nRet;
}

size_t fseekToHeader(FILE* fp)
{
	size_t size = 0;
	unsigned char buf = 0;
	while (!feof(fp) && !ferror(fp)) {
		size += fread(&buf, 1, 1, fp);
		if (isHeader(buf)) {
			for (;;) {
				size += fread(&buf, 1, 1, fp);
				if (!isHeader(buf)) {
					fseek(fp, -1, SEEK_CUR);
					size--;
					break;
				}
			}
			if (0 < size && size < 1340) {
				long size2 = (long)(1340 - size - 1);
//				printf("size:%d,size2:%d\n", size, size2);
				fseek(fp, size2, SEEK_CUR);
				fread(&buf, 1, 1, fp);

				if (!isHeader(buf)) {
					fseek(fp, -size2 - 1, SEEK_CUR);
				}
				else {
					size += size2;
				}
			}
			if (isFooter(buf)) {
				fseek(fp, 1339, SEEK_CUR);
				size += 1339;
				break;
			}
			break;
		}
	}
	return size;
}

int xtract()
{
	FILE * fin;
	FILE * fout;
	unsigned char buffer;
//	unsigned char low;
//	unsigned char high;
	int times;
	long size;
	long tt;
	char fname[20];
	long ts = 0;
	printf("PVD Frame Extraction Utility By Tyler Knott\n");
	fin = fopen("input-raw-left.raw", "rb");
	if (!fin) exit(1);
	fseek(fin, 0, 2);
	size = ftell(fin);
	rewind(fin);
//	fseek(fin, 6402, 0);
//	size = size - 6402;
	// skip pregap (if exists) and first header (1340 bytes)
	size_t seeksize = fseekToHeader(fin);
	printf("Seek size: %zd\n", seeksize);
	size = (long)(size - seeksize - 1340);

	for (tt = size / 5880; tt > 0; tt--)
	{
		ts += 1;
		printf("\rProcessing frame number %li", ts);
		sprintf(fname, "%li.pgm", ts);
		fout = fopen(fname, "wb");
		fputs("P5\n40 80\n255\n", fout);
		for (times = 3200; times != 0; times--)
		{
			fread(&buffer, 1, 1, fin);
			/* low = buffer;
			 high = buffer;
			 low <<= 4;
			 low >>= 4;
			 high >>= 4;
			 if(low > 15) printf("COMPUTATION ERROR!!!  FILE OUTPUT WILL BE INVALID.");
			 fprintf(fout, " %i", low);
			 fprintf(fout, " %i", high); */
			if (buffer == 0xe1) {
				unsigned char nextBuf = 0;
				fread(&nextBuf, 1, 1, fin);
				if (nextBuf == 0xe1) {
					fputc(0, fout);
					printf("\n%li.pgm %d: padded by 0", ts, 3200 - times);
					fseek(fin, -2, SEEK_CUR);
				}
				else {
					fwrite(&buffer, 1, 1, fout);
					fseek(fin, -1, SEEK_CUR);
				}
			}
			else {
				fwrite(&buffer, 1, 1, fout);
			}
		}
		fclose(fout);
		// skip footer (1340 or lower bytes)
		do {
			fread(&buffer, 1, 1, fin);
		} while (isFooter(buffer));
		fseek(fin, -1, SEEK_CUR);

		fread(&buffer, 1, 1, fin);
		// skip header
		if (isHeader(buffer)) {
			fseek(fin, 1339, 1);
		}
	}
	printf("\n");
	fclose(fin);
	remove("input-raw-left.raw");
	return 0;
}

int splitColorXp(char* rawfile)
{
	FILE * fin;
	FILE * foutr;
	FILE * foutl;
	char l1[9] = {};
	char r1;
	long finSize;

	fin = fopen(rawfile, "rb");
	if (!fin) { printf("No input!!!"); exit(2); }
	foutl = fopen("input-raw-left2.raw", "wb");
	foutr = fopen("input-raw-right2.raw", "wb");
	fseek(fin, 0, 2);
	finSize = ftell(fin);
	fseek(fin, 0, 0);
	for (int i = 0; i < finSize; i += 10) {
		fread(&l1, 1, 9, fin);
		fwrite(&l1, 1, 9, foutl);
		fread(&r1, 1, 1, fin);
		fwrite(&r1, 1, 1, foutr);
	}
	fclose(fin);
	fclose(foutr);
	fclose(foutl);
	return 0;
}

int addhColorXp(long frmSize, long audioSizePerFrm, size_t seekPos)
{
	UNREFERENCED_PARAMETER(seekPos);
	FILE* fin;
	FILE* fout;
	long finSize;
	char buffer;
	fin = fopen("input-raw-right2.raw", "rb");
	if (!fin) { printf("ERROR!"); exit(2); }
	char riff[] = "RIFF";
	char wave[] = "WAVE";
	char fmt[] = "fmt ";
	long sc1size = 16;
	int audiofmt = 1;
	int numchanels = 2;
	long smprate = 17640;
	long byterate = 35280;
	int blkalign = 1;
	int bits = 8;
	char dataa[] = "data";
	fout = fopen("input-right2.wav", "wb");
	fwrite(riff, 4, 1, fout);
	fseek(fin, 0, 2);
	finSize = frmSize * audioSizePerFrm;
	fseek(fin, 0, 0);
	long finSizea;
	finSizea = finSize + 40;
	fwrite(&finSizea, 4, 1, fout);
	fwrite(wave, 4, 1, fout);
	fwrite(fmt, 4, 1, fout);
	fwrite(&sc1size, 4, 1, fout);
	fwrite(&audiofmt, 2, 1, fout);
	fwrite(&numchanels, 2, 1, fout);
	fwrite(&smprate, 4, 1, fout);
	fwrite(&byterate, 4, 1, fout);
	fwrite(&blkalign, 2, 1, fout);
	fwrite(&bits, 2, 1, fout);
	fwrite(dataa, 4, 1, fout);
	fwrite(&finSize, 4, 1, fout);

	long tt = 0;
	for (tt = frmSize; tt > 0; tt--) {
		for (int i = 0; i < audioSizePerFrm; i++) {
			fread(&buffer, 1, 1, fin);
			fwrite(&buffer, 1, 1, fout);
		}
	}
	fclose(fin);
	fclose(fout);
	remove("input-raw-right2.raw");
	return 0;
}

int isColor(unsigned char* buf)
{
	int nRet = 0;
	if (buf[0] == 0x81 || buf[1] == 0xe3 || buf[2] == 0xe3 ||
		buf[3] == 0xc7 || buf[4] == 0xc7 || buf[5] == 0x81 ||
		buf[6] == 0x81 || buf[7] == 0xe3 || buf[8] == 0xc7) {
		nRet = 1;
	}
	return nRet;

}

size_t fseekToColor(FILE* fp)
{
	size_t size = 0;
	unsigned char buf[9] = {};
	while (!feof(fp) && !ferror(fp)) {
		size += fread(buf, 1, 9, fp);
		if (isColor(buf)) {
			fseek(fp, 351, SEEK_CUR);
			size += 351;
			break;
		}
		fseek(fp, -8, SEEK_CUR);
	}
	return size;
}

int drawRGB(unsigned char* video, int* idx, long ts)
{
	char fname[20];
	FILE* fout;
	char writeBuf[12] = {};
	int nextline = 108;

	printf("\rProcessing frame number %li", ts);
	sprintf(fname, "%li.ppm", ts);
	fout = fopen(fname, "wb");
	fputs("P6\n144 80\n255\n", fout);

	int nRoop = 0;
	//  https://forum.videohelp.com/threads/123262-converting-video-formats-(For-Hasbro-s-VideoNow)-I-know-the/page17#post1150041
	for (int i = 0; i < 17280 / 6; i++) {
		writeBuf[0] = video[*idx] & 0x0f | video[*idx] << 4 & 0xf0;
		writeBuf[1] = video[*idx + nextline] & 0x0f | video[*idx + nextline] << 4 & 0xf0;
		writeBuf[2] = video[*idx + nextline] & 0xf0 | video[*idx + nextline] >> 4 & 0x0f;

		writeBuf[3] = video[*idx + nextline + 1] & 0x0f | video[*idx + nextline + 1] << 4 & 0xf0;
		writeBuf[4] = video[*idx] & 0xf0 | video[*idx] >> 4 & 0x0f;
		writeBuf[5] = video[*idx + 1] & 0x0f | video[*idx + 1] << 4 & 0xf0;

		writeBuf[6] = video[*idx + 1] & 0xf0 | video[*idx] >> 4 & 0x0f;
		writeBuf[7] = video[*idx + nextline + 1] & 0xf0 | video[*idx + nextline + 1] >> 4 & 0x0f;
		writeBuf[8] = video[*idx + nextline + 2] & 0x0f | video[*idx + nextline + 2] << 4 & 0xf0;

		writeBuf[9] = video[*idx + nextline + 2] & 0xf0 | video[*idx + nextline + 2] >> 4 & 0x0f;
		writeBuf[10] = video[*idx + 2] & 0x0f | video[*idx + 2] << 4 & 0xf0;
		writeBuf[11] = video[*idx + 2] & 0xf0 | video[*idx + 2] >> 4 & 0x0f;
		fwrite(writeBuf, sizeof(writeBuf), 1, fout);

		*idx += 3;
		if (++nRoop == 36) {
			*idx += nextline;
			nRoop = 0;
		}
	}
	fclose(fout);
	return 0;
}

int xtractColor(long* frmSize, size_t* seekPos)
{
	FILE * fin;
	long size;
	long tt;
	long ts = 0;
	
	fin = fopen("input-raw-left2.raw", "rb");
	if (!fin) exit(1);
	fseek(fin, 0, 2);
	size = ftell(fin);
	rewind(fin);
	unsigned char* buf = (unsigned char*)malloc((size_t)size);
	if (!buf) exit(1);
	fread(buf, (size_t)size, 1, fin);
	rewind(fin);
	*seekPos = fseekToColor(fin) - 360;
	printf("Seek size: %zd\n", *seekPos);
	size = (long)(size - *seekPos);

	*frmSize = size / 17640;
	unsigned char* video = &buf[*seekPos];
	int idx = 0;
	for (tt = *frmSize; tt > 0; tt--)
	{
		idx += 360;
		drawRGB(video, &idx, ++ts);
	}
	printf("\n");
	fclose(fin);
	remove("input-raw-left2.raw");
	return 0;
}

size_t fseekToXp(FILE* fp)
{
	size_t size = 0;
	unsigned char buf[9] = {};
	while (!feof(fp) && !ferror(fp)) {
		size += fread(buf, 1, 9, fp);
		if (isColor(buf)) {
			fseek(fp, 495, SEEK_CUR);
			size += 495;
			break;
		}
		fseek(fp, -8, SEEK_CUR);
	}
	return size;
}

int xtractXp(long* frmSize, size_t* seekPos)
{
	FILE * fin;
	long size;
	long tt;
	long ts = 0;

	fin = fopen("input-raw-left2.raw", "rb");
	if (!fin) exit(1);
	fseek(fin, 0, 2);
	size = ftell(fin);
	rewind(fin);
	unsigned char* buf = (unsigned char*)malloc((size_t)size);
	if (!buf) exit(1);
	fread(buf, (size_t)size, 1, fin);
	rewind(fin);
	*seekPos = fseekToXp(fin) - 504;
	printf("Seek size: %zd\n", *seekPos);
	size = (long)(size - *seekPos);

	*frmSize = size / 17784;
	unsigned char* video = &buf[*seekPos];
	int idx = 0;
	for (tt = *frmSize; tt > 0; tt--)
	{
		idx += 504;
		drawRGB(video, &idx, ++ts);
	}
	printf("\n");
	fclose(fin);
	remove("input-raw-left2.raw");
	return 0;
}

int main(int argc, char *argv[])
{
	if (argc != 3) {
		printf(
			"This is a collection of tools to decode VideoNow PVD discs.\n"
			"Usage\n"
			"\t PVDTools.exe bw <raw file>\n"
			"\t\t Output pgm (40 x 80 gray scale) and wav file\n"
			"\t PVDTools.exe color <raw file>\n"
			"\t\t Output ppm (144 x 80 full color) and wav file\n"
			"\t PVDTools.exe xp <raw file>\n"
			"\t\t Output ppm (144 x 80 full color) and wav file\n"
		);
	}
	else {
		long frmSize = 0;
		size_t seekPos = 0;
		if (!strcmp(argv[1], "bw")) {
			split16(argv[2]);
			split8();
			addh();
			xtract();
		}
		else if (!strcmp(argv[1], "color")) {
			splitColorXp(argv[2]);
			xtractColor(&frmSize, &seekPos);
			addhColorXp(frmSize, 1960, seekPos);
		}
		else if (!strcmp(argv[1], "xp")) {
			splitColorXp(argv[2]);
			xtractXp(&frmSize, &seekPos);
			addhColorXp(frmSize, 1976, seekPos);
		}
	}
	return 0;
}
