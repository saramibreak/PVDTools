/**
 * Copyright 2020 sarami
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
#pragma once
#pragma warning(disable:4514 4710 4711)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TRACK 100
#define MAX_INDEX 100

int MSFtoLBA(
	unsigned char byMinute,
	unsigned char bySecond,
	unsigned char byFrame
);

bool HandleCueFile(FILE* fp, char* fname, int lba[MAX_TRACK][MAX_INDEX]);
