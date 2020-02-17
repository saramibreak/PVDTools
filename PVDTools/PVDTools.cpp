/* This program is free software; you can redistribute it and/or modify it under the terms of teh GNU General Public LIcense as published by the Free Software Foundation; either version 2 of teh License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

Copyright (c) 2003 Tyler Knott, 2020 Sarami*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma warning(disable:4996)

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
	printf("File size: %d\n", finSize);
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
	buffer = (char*)malloc(finSize);
	if (buffer) {
		fread(buffer, 1, finSize, fin);
		fwrite(buffer, 1, finSize, fout);
	}
	fclose(fin);
	fclose(fout);
	remove("input-raw-right2.raw");
	return 0;
}

int isHeader(char buf)
{
	int nRet = 0;
	if (buf == (char)0xe1 || buf == (char)0xc3 || buf == (char)0xa5) {
		nRet = 1;
	}
	return nRet;
}

int isFooter(char buf)
{
	int nRet = 0;
	if (buf == (char)0xd2 || buf == (char)0xb4 || buf == (char)0x96) {
		nRet = 1;
	}
	return nRet;
}

size_t fseekToHeader(FILE* fp)
{
	int nRet = 0;
	size_t size = 0;
	char buf = 0;
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
				int size2 = 1340 - size - 1;
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
	char buffer;
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
	printf("Seek size: %d\n", seeksize);
	size = size - seeksize - 1340;

	for (tt = size / 5880; tt > 0; tt--)
	{
		ts += 1;
		printf("\rProcessing frame number %li", ts);
		sprintf(fname, "%i.pgm", ts);
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
			if (buffer == (char)0xe1) {
				char nextBuf = 0;
				fread(&nextBuf, 1, 1, fin);
				if (nextBuf == (char)0xe1) {
					fputc(0, fout);
					printf("\n%i.pgm %d: padded by 0", ts, 3200 - times);
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

int splitm(char* rawfile)
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

int addh2()
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
	int numchanels = 2;
	long smprate = 17640;
	long byterate = 35280;
	int blkalign = 1;
	int bits = 8;
	char dataa[] = "data";
	fout = fopen("input-right2.wav", "wb");
	fwrite(riff, 4, 1, fout);
	fseek(fin, 0, 2);
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
	buffer = (char*)malloc(finSize);
	if (buffer) {
		fread(buffer, 1, finSize, fin);
		fwrite(buffer, 1, finSize, fout);
	}
	fclose(fin);
	fclose(fout);
	remove("input-raw-right2.raw");
	return 0;
}

int isColor(char* buf)
{
	int nRet = 0;
	if (buf[0] == (char)0x81 || buf[1] == (char)0xe3 || buf[2] == (char)0xe3 ||
		buf[3] == (char)0xc7 || buf[4] == (char)0xc7 || buf[5] == (char)0x81 ||
		buf[6] == (char)0x81 || buf[7] == (char)0xe3 || buf[8] == (char)0xc7) {
		nRet = 1;
	}
	return nRet;

}

size_t fseekToColor(FILE* fp)
{
	size_t size = 0;
	char buf[9] = {};
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

int xtractColor(bool isGray)
{
	FILE * fin;
	FILE * fout;
	char buffer;
	long size;
	long tt;
	char fname[20];
	long ts = 0;
	
	fin = fopen("input-raw-left2.raw", "rb");
	if (!fin) exit(1);
	fseek(fin, 0, 2);
	size = ftell(fin);
	rewind(fin);
	size_t seeksize = fseekToColor(fin);
	printf("Seek size: %d\n", seeksize);
	size = size - seeksize + 360;

	for (tt = size / 17640; tt > 0; tt--)
	{
		ts += 1;
		printf("\rProcessing frame number %li", ts);
		if (isGray) {
			sprintf(fname, "%i.pgm", ts);
		}
		else {
			sprintf(fname, "%i.ppm", ts);
		}
		fout = fopen(fname, "wb");
		if (isGray) {
			fputs("P5\n108 160\n255\n", fout);
		}
		else {
			fputs("P6\n36 160\n255\n", fout);
		}
		
		for (int i = 0; i < 17280; i++) {
			fread(&buffer, 1, 1, fin);
			fwrite(&buffer, 1, 1, fout);
		}
		fclose(fout);
		fseek(fin, 360, SEEK_CUR);
	}
	printf("\n");
	fclose(fin);
	remove("input-raw-left2.raw");
	return 0;
}

size_t fseekToXp(FILE* fp)
{
	size_t size = 0;
	char buf[9] = {};
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

int xtractXp(bool isGray)
{
	FILE * fin;
	FILE * fout;
	char buffer;
	long size;
	long tt;
	char fname[20];
	long ts = 0;

	fin = fopen("input-raw-left2.raw", "rb");
	if (!fin) exit(1);
	fseek(fin, 0, 2);
	size = ftell(fin);
	rewind(fin);
	size_t seeksize = fseekToXp(fin);
	printf("Seek size: %d\n", seeksize);
	size = size - seeksize + 504;

	for (tt = size / 17784; tt > 0; tt--)
	{
		ts += 1;
		printf("\rProcessing frame number %li", ts);
		if (isGray) {
			sprintf(fname, "%i.pgm", ts);
		}
		else {
			sprintf(fname, "%i.ppm", ts);
		}
		fout = fopen(fname, "wb");
		if (isGray) {
			fputs("P5\n108 160\n255\n", fout);
		}
		else {
			fputs("P6\n36 160\n255\n", fout);
		}

		for (int i = 0; i < 17280; i++) {
			fread(&buffer, 1, 1, fin);
			fwrite(&buffer, 1, 1, fout);
		}
		fclose(fout);
		fseek(fin, 504, SEEK_CUR);
	}
	printf("\n");
	fclose(fin);
	remove("input-raw-left2.raw");
	return 0;
}

int main(int argc, char *argv[])
{
	if (argc != 3 && argc != 4) {
		printf(
			"This is a collection of tools to decode VideoNow PVD discs.\n"
			"Usage\n"
			"\t PVDTools.exe bw <raw file>\n"
			"\t\t Output pgm (40 x 80 gray scale) and wav file\n"
			"\t PVDTools.exe color <raw file>\n"
			"\t\t Output ppm (36 x 160 incomplete color) and wav file\n"
			"\t PVDTools.exe color <raw file> /g\n"
			"\t\t Output pgm (108 x 160 gray scale) and wav file\n"
			"\t PVDTools.exe xp <raw file>\n"
			"\t\t Output ppm (36 x 160 incomplete color) and wav file\n"
			"\t PVDTools.exe xp <raw file> /g\n"
			"\t\t Output pgm (108 x 160 gray scale) and wav file\n"
		);
	}
	else {
		if (!strcmp(argv[1], "bw")) {
			split16(argv[2]);
			split8();
			addh();
			xtract();
		}
		else if (!strcmp(argv[1], "color")) {
			splitm(argv[2]);
			addh2();
			if (argc == 4 && !strncmp(argv[3], "/g", 2)) {
				xtractColor(true);
			}
			else {
				xtractColor(false);
			}
		}
		else if (!strcmp(argv[1], "xp")) {
			splitm(argv[2]);
			addh2();
			if (argc == 4 && !strncmp(argv[3], "/g", 2)) {
				xtractXp(true);
			}
			else {
				xtractXp(false);
			}
		}
	}
	return 0;
}
