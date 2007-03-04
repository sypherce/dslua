#include <stdio.h>
#include <stdlib.h>

signed long findString (char *searchingForThis, int sizeFor, char* searchingInThis, int sizeIn)
{
    signed long i = 0;
    for(i=0;i<sizeIn-sizeFor;i++)
    {
        if(searchingInThis[i] == searchingForThis[0])
        {
            signed long i2 = 0;
            for(i2=0;i2<sizeFor;i2++)
            {
                if(searchingForThis[i2] != searchingInThis[i+i2])
                    i2 = sizeFor;
                else if(i2 == sizeFor-1)
                {
                    return i;
                }
            }
        }
    }
    return -1;
}

int main(int argc, char *argv[])
{
    if(argc == 4)
    {
    char *originalLuaPath = "___FOLDER______________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________";
    long bufferSize;
    char *buffer;
    char ndsFile[255];
    char ndsOutFile[255];
    char path[255];
    strncpy(ndsFile, argv[1], 255);
    strncpy(ndsOutFile, argv[2], 255);
    strncpy(path, argv[3], 255);
    FILE *pFile = fopen(ndsFile, "rb");
      if (pFile==NULL)
      {
          printf("Could not open file \"%s\".", ndsFile);
          exit (1);
      }

  // obtain file size.
  fseek (pFile , 0 , SEEK_END);
  bufferSize = ftell (pFile);
  rewind (pFile);

  // allocate memory to contain the whole file.
  buffer = (char*) malloc (bufferSize);
  if (buffer == NULL)
  {
          printf("Could not load file into memory.");
              exit (2);
             }

  // copy the file into the buffer.
  fread (buffer,1,bufferSize,pFile);

  /*** the whole file is loaded in the buffer. ***/

    fclose (pFile);
    int qi = 0;
    int locationPath = findString(originalLuaPath, strlen(originalLuaPath), buffer, bufferSize);
    for(qi = 0; qi < 255; qi++)
    {
        buffer[locationPath+qi] = path[qi];
        if(path[qi] == 0)
            break;
    }
    
    pFile = fopen(ndsOutFile, "wb");
      if (pFile==NULL)
      {
          printf("Could not create file \"%s\".", ndsOutFile);
          exit (1);
      }
      

  // copy the file into the buffer.
  fwrite (buffer,1,bufferSize,pFile);

  /*** the whole file is loaded in the buffer. ***/

    fclose (pFile);
    printf("Sucessfully created AutoRun patched File");
    }
    else
    {
        printf("Usage: autopatch.exe [InputFile] [OutputFile] [PathTo]\n");
        return 1;
    }

    return 0;
}
