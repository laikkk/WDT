/*	Copyright (C) 2014 by Aleksander Denisiuk

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>


#include "Texture.h"

typedef struct TGAHeader
{
  uint8_t  idlength;
  uint8_t  colormap;
  uint8_t  datatype;
  uint8_t  colormapinfo[5];
  uint16_t xorigin;
  uint16_t yorigin;
  uint16_t width;
  uint16_t height;
  uint8_t  bitperpel;
  uint8_t  description;
} TGAHeader;


void CreateTextureFromTGA(Texture * tex, const char * filename){

  TGAHeader *header;
  GLchar   *buffer;
  GLint     format, internalFormat;


  FILE* file;
  long size = -1;

  if (NULL != (file = fopen(filename, "rb")) &&
    0 == fseek(file, 0, SEEK_END) &&
    -1 != (size = ftell(file)))
  {
    rewind(file);

    if (NULL != (buffer = (GLchar*)malloc(size))){
      if (size != (long)fread(buffer, sizeof(GLchar), size, file)){
			fprintf(stderr, "ERROR: Could not read file %s\n", filename);
			free(buffer);
			fclose(file);
			exit(EXIT_FAILURE);
      }
    }
    else {
      fprintf(stderr, "ERROR: Could not allocate %li bytes.\n", size);
      fclose(file);
      exit(EXIT_FAILURE);
    }
    fclose(file);
  }
  else{
    fprintf(stderr, "ERROR: Could not open file %s\n", filename);
    exit(EXIT_FAILURE);
  }


  // Rozmiar jest mniejszy od nagłówka
    if (size <= sizeof(TGAHeader))
    {
      fprintf(stderr, "Too small file '%s'\n", filename);
      free(buffer);
      exit(EXIT_FAILURE);
    }

    header = (TGAHeader*)buffer;

    // sprawda się plik TGA: nieskompresowany RGB lub RGBA obraz
    if (header->datatype != 2 || (header->bitperpel != 24 && header->bitperpel != 32))
    {
      fprintf(stderr, "Wrong TGA format '%s'\n", filename);
      free(buffer);
      exit(EXIT_FAILURE);

    }

    // format tekstury
    format = (header->bitperpel == 24 ? GL_BGR : GL_BGRA);
    internalFormat = (format == GL_BGR ? GL_RGB8 : GL_RGBA8);


    // nowy indeks tekstury
    glGenTextures(1, &(tex->Texture));


    // aktywizacja
    glBindTexture(GL_TEXTURE_2D, tex->Texture);


    // wysyłanie tekstury na GPU
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, header->width, header->height, 0, format,
		 GL_UNSIGNED_BYTE, (const GLvoid*)(buffer + sizeof(TGAHeader) + header->idlength));


    // zwolnienie pamięci
    free(buffer);

    //
    ExitOnGLError("ERROR: Could not load texture");

}


void DestroyTexture(Texture * tex){
  glDeleteTextures(1, &(tex->Texture));
  ExitOnGLError("ERROR: Could not destroy the texture");
}	
