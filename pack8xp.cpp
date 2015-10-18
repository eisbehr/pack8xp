#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<stdint.h>
typedef float r32;
typedef double r64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int32_t b32;

#define ERROR(message)					\
  {							\
   printf("E %s:%i %s\n", __FILE__, __LINE__, message);	\
    exit(1);						\
  }

#pragma pack(push, 1)
struct Header
{
  char signature[8]; // **TIFL*
  u8 moreSig[3]; // 0x1A, 0x0A, 0x00 which is 26, 10, 0
  char comment[42]; // something we can choose.
  u16 sizeDataSection; // File size - 57
  // Data section
};

struct Entry
{
  u16 id; // either 11 or 13; 0x0B or 0x0D (we use the latter).
  u16 sizeEntryData; // size of the binary blob from the assembler plus two checksum bytes
  u8 typeId; // always 6 for us?
  char name[8]; // name that shows up in the calculator
  u8 version; // just set to 0
  u8 flag; // also 0
  u16 sizeEntryData2; // Duplicate of the first, for some reason
  u16 sizeEntryDataPlain; // size just of the data without counting the checksum
                          // this is part of the "Y-variable" format used for programs.
  // data here, this is just the binary blob produced by the assembler
  // two bytes checksum. Lower 16 bit of sum of all data bytes.
};
#pragma pack(pop)

b32
isAlphaNum(char* string)
{
  char* p = string;
  while(*p != 0) 
    {
      if((*p >= 48 && *p <= 57) ||
	(*p >= 65 && *p <= 90) ||
	 (*p >= 97 && *p <= 122))
	{
	  // is alphanum
	}
      else
	{
	  return 0;
	}
      p++;
    }
  return 1;
}

void
toUpper(char* string)
{
  char* p = string;
  while(*p != 0)
    {
      if((*p >= 97 && *p <= 122))
	{
	  // is lower case
	  *p = *p - 32;
	}
      p++;
    }
}

u16
checkSum(void* data, int dataSize)
{
  u32 byteSum = 0;
  u8* d = (u8*)data;
  for(int i=0; i<dataSize; i++)
    {
      byteSum += d[i];
    }
  return (u16)byteSum;
}

void
printHelp(char* progname)
{
  printf("Usage: %s [input file] [program name]\n\n", progname);
  
  printf("The input file must be raw Z80 machine code without any headers, checksums, etc.\n");
}

i32 main(i32 argc, char** argv)
{
  char* fileArg = argv[1];
  char* outFilenameBase = argv[2];

  if(fileArg == 0 || outFilenameBase == 0) 
    {
      printHelp(argv[0]); 
      exit(0);
    }

  if(!isAlphaNum(outFilenameBase)) ERROR("Output name has to be alphanumeric");
  toUpper(outFilenameBase);

  FILE* handle = fopen(fileArg, "rb");
  if(!handle) ERROR("File could not be opened");

  fseek(handle, 0, SEEK_END);
  int fileArgSize = ftell(handle);
  fseek(handle, 0, SEEK_SET);

  void* fileArgBlob = malloc(fileArgSize);
  if(fread(fileArgBlob, fileArgSize, 1, handle) != 1) ERROR("File could not be read");
  fclose(handle);

  int outFileSize = 
    sizeof(Header) + 2 /* checksum */ + 
    sizeof(Entry) + fileArgSize;

  void* outFile = malloc(outFileSize);

  Header* header = (Header*) outFile;
  strncpy(header->signature, "**TI83F*", 8);
  u8 tmpSig[3] = { 0x1A, 0x0A, 0x00 };
  memcpy(header->moreSig, tmpSig, sizeof(tmpSig));
  const char* comment = "Created with http://pack8xp.eisbehr.org";
  int commentLength = strlen(comment);

  if(commentLength > 42) ERROR("Given comment too long. Must me 42 bytes or less");
  strncpy(header->comment, comment, 42);
  header->sizeDataSection = outFileSize - sizeof(Header) - 2 /* checksum */;

  Entry* entry = (Entry*) ((u8*)outFile + sizeof(Header));
  entry->id = 0x0D;
  entry->sizeEntryData = fileArgSize+2;
  entry->typeId = 6;
  strncpy(entry->name, outFilenameBase, 8); // This is fine, we don't need it to null-terminate

  entry->version = 0;
  entry->flag = 0;
  entry->sizeEntryData2 = entry->sizeEntryData;
  entry->sizeEntryDataPlain = fileArgSize;

  u8* dataPointer = ((u8*)entry + sizeof(Entry)); // why +2?
  memcpy(dataPointer, fileArgBlob, fileArgSize);

  dataPointer += fileArgSize;
  *((u16*)dataPointer) = checkSum(entry, fileArgSize + sizeof(Entry));

  int outFilenameBaseLength = strlen(outFilenameBase);
  char* outFilename = (char*)malloc(outFilenameBaseLength+1+4);
  strcpy(outFilename, outFilenameBase);
  strncpy(outFilename+outFilenameBaseLength, ".8XP", 5);
  
  FILE* outHandle = fopen(outFilename, "wb");
  if(!outHandle) ERROR("Output file could not be openend");
  if(fwrite(outFile, outFileSize, 1, outHandle) != 1) ERROR("Output file could not be written");
  fclose(outHandle);

  printf("File created as %s\n", outFilename);
  return 0;
} 
