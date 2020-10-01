/**
 * Copyright 2019-2020 sarami
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma warning(disable:5045)
#pragma warning(push)
#pragma warning(disable:4710 4820)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#pragma warning(pop)
#include "cuefileParser.h"

#define UNREFERENCED_PARAMETER(P) (P)

int addh()
{
	FILE * fin;
	FILE * fout;
	long finSize;
	char * buffer;
	fin = fopen("input-raw-right2.raw", "rb");
	if (!fin) {
		printf("ERROR!");
		exit(2);
	}
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

int xtract(long* ts)
{
	FILE * fin;
	FILE * fout;
	unsigned char buffer;
	int times;
	long size;
	long tt;
	char fname[20];

	fin = fopen("input-raw-left.raw", "rb");
	if (!fin) {
		exit(1);
	}
	fseek(fin, 0, 2);
	size = ftell(fin);
	rewind(fin);

	// skip pregap (if exists) and first header (1340 bytes)
	size_t seeksize = fseekToHeader(fin);
	printf("Seek size: %zd\n", seeksize);
	size = (long)(size - seeksize - 1340);

	for (tt = size / 5880; tt > 0; tt--) {
		*ts += 1;
		printf("\rProcessing frame number %li", *ts);
		sprintf(fname, "%li.pgm", *ts);
		fout = fopen(fname, "wb");
		fputs("P5\n40 80\n255\n", fout);
		for (times = 3200; times != 0; times--) {
			fread(&buffer, 1, 1, fin);
			if (buffer == 0xe1) {
				unsigned char nextBuf = 0;
				fread(&nextBuf, 1, 1, fin);
				if (nextBuf == 0xe1) {
					fputc(0, fout);
					printf("\n%li.pgm %d: padded by 0", *ts, 3200 - times);
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

int addhColorXp(long frmSize, long audioSizePerFrm, size_t seekPos)
{
	UNREFERENCED_PARAMETER(seekPos);
	FILE* fin;
	FILE* fout;
	long finSize;
	char buffer;
	fin = fopen("input-raw-right2.raw", "rb");
	if (!fin) {
		printf("ERROR!");
		exit(2);
	}
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

int outputPpm(unsigned char* video, int* idx, long ts)
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

int isColor(unsigned char* buf)
{
	int nRet = 0;
	if (buf[0] == 0x81 && buf[1] == 0xe3 && buf[2] == 0xe3 &&
		buf[3] == 0xc7 && buf[4] == 0xc7 && buf[5] == 0x81 &&
		buf[6] == 0x81 && buf[7] == 0xe3 && buf[8] == 0xc7) {
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
		size -= 8;
	}
	return size;
}

int xtractColor(long* frmSize, size_t* seekPos, long* ts)
{
	FILE * fin;
	long size;
	long tt;
	int fd;
	struct _stat32 stbuf;

	fd = _open("input-raw-left2.raw", _O_RDONLY | _O_BINARY);
	if (!fd) {
		exit(1);
	}
	fin = _fdopen(fd, "rb");
	if (!fin) {
		exit(1);
	}
	_fstat32(fd, &stbuf);

	unsigned char* buf = (unsigned char*)malloc((size_t)stbuf.st_size);
	if (!buf) {
		exit(1);
	}
	fread(buf, 1, (size_t)stbuf.st_size, fin);
	rewind(fin);

	*seekPos = fseekToColor(fin) - 360;
	printf("Seek size: %zd\n", *seekPos);
	size = (long)(stbuf.st_size - *seekPos);

	*frmSize = (long)(size / 17640);
	unsigned char* video = &buf[*seekPos];
	int idx = 0;
	for (tt = *frmSize; tt > 0; tt--) {
		idx += 360;
		outputPpm(video, &idx, ++(*ts));
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
		size -= 8;
	}
	return size;
}

int xtractXp(long* frmSize, size_t* seekPos, long* ts)
{
	FILE * fin;
	long size;
	long tt;
	int fd;
	struct _stat32 stbuf;

	fd = _open("input-raw-left2.raw", _O_RDONLY | _O_BINARY);
	if (!fd) {
		exit(1);
	}
	fin = _fdopen(fd, "rb");
	if (!fin) {
		exit(1);
	}
	_fstat32(fd, &stbuf);

	unsigned char* buf = (unsigned char*)malloc((size_t)stbuf.st_size);
	if (!buf) {
		exit(1);
	}
	fread(buf, 1, (size_t)stbuf.st_size, fin);
	rewind(fin);

	*seekPos = fseekToXp(fin) - 504;
	printf("Seek size: %zd\n", *seekPos);
	size = (long)(stbuf.st_size - *seekPos);

	*frmSize = size / 17784;
	unsigned char* video = &buf[*seekPos];
	int idx = 0;
	for (tt = *frmSize; tt > 0; tt--) {
		idx += 504;
		outputPpm(video, &idx, ++(*ts));
	}
	printf("\n");
	fclose(fin);
	remove("input-raw-left2.raw");
	return 0;
}

int isColorOrXp(FILE* fp, long long* offset)
{
	unsigned char buf[9] = {};
	int nCnt = 0;
	while (!feof(fp) && !ferror(fp)) {
		*offset += fread(buf, 1, 9, fp);
		if (isColor(buf)) {
			nCnt++;
			for (int i = 0; i < 23; i++) {
				fread(buf, 1, 1, fp); // skip audio byte
				fread(buf, 1, 9, fp);
				if (isColor(buf)) {
					nCnt++;
				}
			}
			*offset -= 9;
			_fseeki64(fp, (long)*offset, SEEK_SET);
			break;
		}
		fseek(fp, -8, SEEK_CUR);
		*offset -= 8;
	}
	return nCnt;
}

int handleColorOrXp(char* arg, FILE* fpImg, long fsize, int lba[MAX_TRACK][MAX_INDEX])
{
	FILE* foutr = fopen("input-raw-right2.raw", "wb");
	unsigned char l1[9] = {};
	unsigned char r1;
	int type = 0;
	long frmSize = 0;
	int allFrmSize = 0;
	long ppmNum = 0;
	size_t seekPos = 0;

	for (int i = 1; lba[i][1] != -1; i++) {
		FILE* foutl = fopen("input-raw-left2.raw", "wb");
		printf("Track[%02d] LBA: %6d, ", i, lba[i - 1][1]);
		long long offset = lba[i - 1][1] * 2352;
		_fseeki64(fpImg, offset, SEEK_SET);

		type = isColorOrXp(fpImg, &offset);
		frmSize = 0;
		if (type == 24) {
			printf("color ");
			frmSize = 19600;
		}
		else if (type == 12) {
			printf("   xp ");
			frmSize = 19760;
		}
		else {
			printf("Frames: %5d\n", 0);
			break;
		}
		int sectorNum = lba[i][1] - lba[i - 1][1];
		if (sectorNum == 0) {
			sectorNum = (long)(fsize / 2352 - lba[i - 1][1]);
		}
		int frmNum = sectorNum * 2352 / frmSize;
		printf("Frames: %5d, Bytes: %9ld\n", frmNum, frmNum * frmSize);
		for (int j = 0; j < frmNum * frmSize; j += 10) {
			fread(&l1, 1, 9, fpImg);
			fwrite(&l1, 1, 9, foutl);
			fread(&r1, 1, 1, fpImg);
			fwrite(&r1, 1, 1, foutr);
		}
		fclose(foutl);
		if (type == 24) {
			xtractColor(&frmSize, &seekPos, &ppmNum);
			allFrmSize += frmSize;
		}
		else if (type == 12) {
			xtractXp(&frmSize, &seekPos, &ppmNum);
			allFrmSize += frmSize;
		}
	}
	fclose(foutr);
	if (!strcmp(arg, "color")) {
		addhColorXp(allFrmSize, 1960, seekPos);
	}
	else if (!strcmp(arg, "xp")) {
		addhColorXp(allFrmSize, 1976, seekPos);
	}
	return 0;
}

int handleBlackAndWhite(FILE* fpImg, long fsize, int lba[MAX_TRACK][MAX_INDEX])
{
	FILE* foutl2 = fopen("input-raw-left2.raw", "wb");
	FILE* foutr2 = fopen("input-raw-right2.raw", "wb");
	unsigned char l1[2] = {};
	unsigned char l2;
	unsigned char r2;
	long frmSize = 11760;
	long ppmNum = 0;

	for (int i = 1; lba[i - 1][1] != -1; i++) {
		FILE* foutl = fopen("input-raw-left.raw", "wb");
		printf("Track[%02d] LBA: %6d, ", i, lba[i - 1][1]);
		long long offset = lba[i - 1][1] * 2352;
		_fseeki64(fpImg, offset, SEEK_SET);

		unsigned char buf = 0;
		long readsize = 0;
		while (!feof(fpImg)) {
			readsize += (long)fread(&buf, sizeof(unsigned char), 1, fpImg);
			if (isHeader(buf)) {
				fseek(fpImg, -readsize, SEEK_CUR);
				break;
			}
		}
		int sectorNum = 0;
		if (lba[i][0] != -1 && lba[i - 1][1] != -1) {
			sectorNum = lba[i][0] - lba[i - 1][1];
		}
		else if (lba[i][1] != -1 && lba[i - 1][1] != -1) {
			sectorNum = lba[i][1] - lba[i - 1][1];
		}
		if (sectorNum == 0) {
			sectorNum = (long)(fsize / 2352 - lba[i - 1][1]);
		}
		int frmNum = sectorNum * 2352 / frmSize;
		printf("Frames: %5d, Bytes: %9d\n", frmNum, sectorNum * 2352);
		for (int j = 0; j < sectorNum * 2352; j += 4) {
			fread(&l1, 1, 2, fpImg);
			fwrite(&l1, 1, 2, foutl);
			fread(&l2, 1, 1, fpImg);
			fwrite(&l2, 1, 1, foutl2);
			fread(&r2, 1, 1, fpImg);
			fwrite(&r2, 1, 1, foutr2);
		}
		if (lba[i][0] != -1) {
			// skip pregap
			_fseeki64(fpImg, (long long)((lba[i][1] - lba[i][0]) * 2352), SEEK_CUR);
		}
		fclose(foutl);
		xtract(&ppmNum);
	}
	fclose(foutl2);
	fclose(foutr2);
	addh();
	remove("input-raw-left2.raw");
	return 0;
}

void printUsage()
{
	printf(
		"This is a collection of tools to decode VideoNow PVD discs.\n"
		"Usage\n"
		"\t PVDTools.exe bw <cue file>\n"
		"\t\t Output pgm (40 x 80 gray scale) and wav file\n"
		"\t PVDTools.exe color <cue file>\n"
		"\t\t Output ppm (144 x 80 full color) and wav file\n"
		"\t PVDTools.exe xp <cue file>\n"
		"\t\t Output ppm (144 x 80 full color) and wav file\n"
	);
}

int main(int argc, char *argv[])
{
	if (argc != 3) {
		printUsage();
	}
	else {
		char drive[_MAX_DRIVE] = {};
		char dir[_MAX_DIR] = {};
		char ext[_MAX_EXT] = {};
		_splitpath(argv[2], drive, dir, NULL, ext);
		if (!strcmp(ext, ".cue")) {
			FILE* fp = fopen(argv[2], "r");
			if (!fp) {
				exit(1);
			}
			int lba[MAX_TRACK][MAX_INDEX] = {};
			for (int i = 0; i < MAX_TRACK; i++) {
				for (int j = 0; j < MAX_INDEX; j++) {
					lba[i][j] = -1;
				}
			}
			char fullpath[_MAX_PATH] = {};
			char fname[_MAX_FNAME] = {};
			HandleCueFile(fp, fname, lba);
			fclose(fp);
			_makepath(fullpath, drive, dir, fname, NULL);

			int fd = _open(fullpath, _O_RDONLY | _O_BINARY);
			FILE* fpImg = _fdopen(fd, "rb");
			if (!fpImg) {
				exit(1);
			}
			struct _stat32 stbuf;
			_fstat32(fd, &stbuf);

			if (!strcmp(argv[1], "bw")) {
				handleBlackAndWhite(fpImg, stbuf.st_size, lba);
			}
			else if (!strcmp(argv[1], "color") || !strcmp(argv[1], "xp")) {
				handleColorOrXp(argv[1], fpImg, stbuf.st_size, lba);
			}
			fclose(fpImg);
		}
		else {
			printUsage();
		}
	}
	return 0;
}
