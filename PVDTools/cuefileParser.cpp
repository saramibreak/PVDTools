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
#include "cuefileParser.h"

// 00:00:00 <= MSF <= 89:59:74
// 90:00:00 <= MSF <= 99:59:74 is TODO
int MSFtoLBA(
	unsigned char byMinute,
	unsigned char bySecond,
	unsigned char byFrame
)
{
	return (byMinute * 60 + bySecond) * 75 + byFrame;
}

bool getSkipSyntax(char* str)
{
	const char* p1 = strstr(str, "CATALOG");
	const char* p2 = strstr(str, "CDTEXTFILE");
	const char* p3 = strstr(str, "FLAGS");
	const char* p4 = strstr(str, "ISRC");
	const char* p5 = strstr(str, "PERFORMER");
	const char* p6 = strstr(str, "POSTGAP");
	const char* p7 = strstr(str, "PREGAP");
	const char* p8 = strstr(str, "REM");
	const char* p9 = strstr(str, "SONGWRITER");
	const char* p10 = strstr(str, "TITLE");
	const char* p11 = strstr(str, "TRACK");
	const char* p12 = strstr(str, "AUDIO");
	if (p1 || p2 || p3 || p4 || p5 || p6 ||
		p7 || p8 || p9 || p10 || p11 || p12) {
		return true;
	}
	return false;
}

bool getFileName(char* str, char* fname)
{
	const char* p3 = strstr(str, "\"");
	if (!p3) {
		return false;
	}
	const char* p4 = strstr(p3 + 1, "\"");
	if (!p4) {
		return false;
	}
	size_t flen = (p4 - p3) / sizeof(char) - 1;
	strncpy(fname, p3 + 1, flen);
	return true;
}

bool getFileSyntax(char* str, char* fname)
{
	const char* p1 = strstr(str, "FILE");
	const char* p2 = strstr(str, "BINARY");
	if (!p1 || !p2) {
		return false;
	}
	return getFileName(str, fname);
}

bool getTrackSyntax(char* str, int* trackNum)
{
	const char* p1 = strstr(str, "TRACK");
	if (!p1) {
		return false;
	}
	char i[3] = { 0 };
	i[0] = *(p1 + 6);
	i[1] = *(p1 + 7);
	char* endptr = NULL;
	*trackNum = (unsigned char)strtol(i, &endptr, 10);
	return true;
}

bool getIndexSyntax(char* str, long* idx, int* lba)
{
	const char* pIndex = strstr(str, "INDEX");
	if (!pIndex) {
		return false;
	}
	// get MSF
	char i[3] = { 0 };
	i[0] = *(pIndex + 6);
	i[1] = *(pIndex + 7);
	char* endptr = NULL;
	*idx = strtol(i, &endptr, 10);

	// get MSF
	char m[3] = { 0 };
	m[0] = *(pIndex + 9);
	m[1] = *(pIndex + 10);
	unsigned char min = (unsigned char)strtol(m, &endptr, 10);

	char s[3] = { 0 };
	s[0] = *(pIndex + 12);
	s[1] = *(pIndex + 13);
	unsigned char sec = (unsigned char)strtol(s, &endptr, 10);

	char f[3] = { 0 };
	f[0] = *(pIndex + 15);
	f[1] = *(pIndex + 16);
	unsigned char frm = (unsigned char)strtol(f, &endptr, 10);

	*lba = MSFtoLBA(min, sec, frm);
	return true;
}

bool HandleCueFile(FILE* fp, char* fname, int lba[MAX_TRACK][MAX_INDEX])
{
	char str[256] = { 0 };
	fgets(str, sizeof(str) / sizeof(char), fp);
	while (!feof(fp)) {
		for (;;) {
			if (getSkipSyntax(str)) {
				if (!fgets(str, sizeof(str) / sizeof(char), fp)) {
					fclose(fp);
					return false;
				}
			}
			else {
				break;
			}
		}
		if (!getFileSyntax(str, fname)) {
			break;
		}

		int i = 0;
		for (; i < MAX_TRACK; i++) {
			int track = 0;
			while (!feof(fp)) {
				if (!fgets(str, sizeof(str) / sizeof(char), fp)) {
					break;
				}
				if (getTrackSyntax(str, &track)) {
					long idx = 0;
					int tmplba = 0;
					for (int j = 0; j < MAX_INDEX; j++) {
						if (!fgets(str, sizeof(str) / sizeof(char), fp)) {
							break;
						}
						while (!feof(fp)) {
							if (!getIndexSyntax(str, &idx, &tmplba)) {
								if (!fgets(str, sizeof(str) / sizeof(char), fp)) {
									break;
								}
							}
							else {
								lba[track - 1][idx] = tmplba;
								break;
							}
						}
						if (idx == 1) {
							break;
						}
					}
					break;
				}
			}
		}
	}
	return true;
}
